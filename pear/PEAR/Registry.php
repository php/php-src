<?php
//
// +----------------------------------------------------------------------+
// | PHP Version 4                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2003 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 3.0 of the PHP license,       |
// | that is bundled with this package in the file LICENSE, and is        |
// | available through the world-wide-web at the following url:           |
// | http://www.php.net/license/3_0.txt.                                  |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Author: Stig Bakken <ssb@php.net>                                    |
// |         Tomas V.V.Cox <cox@idecnet.com>                              |
// |                                                                      |
// +----------------------------------------------------------------------+
//
// $Id$

/*
TODO:
    - Transform into singleton()
    - Add application level lock (avoid change the registry from the cmdline
      while using the GTK interface, for ex.)
*/
require_once "System.php";
require_once "PEAR.php";

define('PEAR_REGISTRY_ERROR_LOCK',   -2);
define('PEAR_REGISTRY_ERROR_FORMAT', -3);
define('PEAR_REGISTRY_ERROR_FILE',   -4);

/**
 * Administration class used to maintain the installed package database.
 */
class PEAR_Registry extends PEAR
{
    // {{{ properties

    /** Directory where registry files are stored.
     * @var string
     */
    var $statedir = '';

    /** File where the file map is stored
     * @var string
     */
    var $filemap = '';

    /** Name of file used for locking the registry
     * @var string
     */
    var $lockfile = '';

    /** File descriptor used during locking
     * @var resource
     */
    var $lock_fp = null;

    /** Mode used during locking
     * @var int
     */
    var $lock_mode = 0; // XXX UNUSED

    /** Cache of package information.  Structure:
     * array(
     *   'package' => array('id' => ... ),
     *   ... )
     * @var array
     */
    var $pkginfo_cache = array();

    /** Cache of file map.  Structure:
     * array( '/path/to/file' => 'package', ... )
     * @var array
     */
    var $filemap_cache = array();

    // }}}

    // {{{ constructor

    /**
     * PEAR_Registry constructor.
     *
     * @param string (optional) PEAR install directory (for .php files)
     *
     * @access public
     */
    function PEAR_Registry($pear_install_dir = PEAR_INSTALL_DIR)
    {
        parent::PEAR();
        $ds = DIRECTORY_SEPARATOR;
        $this->install_dir = $pear_install_dir;
        $this->statedir = $pear_install_dir.$ds.'.registry';
        $this->filemap  = $pear_install_dir.$ds.'.filemap';
        $this->lockfile = $pear_install_dir.$ds.'.lock';

        // XXX Compatibility code should be removed in the future
        // rename all registry files if any to lowercase
        if (!OS_WINDOWS && $handle = @opendir($this->statedir)) {
            $dest = $this->statedir . DIRECTORY_SEPARATOR;
            while (false !== ($file = readdir($handle))) {
                if (preg_match('/^.*[A-Z].*\.reg$/', $file)) {
                    rename($dest . $file, $dest . strtolower($file));
                }
            }
            closedir($handle);
        }
        if (!file_exists($this->filemap)) {
            $this->rebuildFileMap();
        }
    }

    // }}}
    // {{{ destructor

    /**
     * PEAR_Registry destructor.  Makes sure no locks are forgotten.
     *
     * @access private
     */
    function _PEAR_Registry()
    {
        parent::_PEAR();
        if (is_resource($this->lock_fp)) {
            $this->_unlock();
        }
    }

    // }}}

    // {{{ _assertStateDir()

    /**
     * Make sure the directory where we keep registry files exists.
     *
     * @return bool TRUE if directory exists, FALSE if it could not be
     * created
     *
     * @access private
     */
    function _assertStateDir()
    {
        if (!@is_dir($this->statedir)) {
            if (!System::mkdir(array('-p', $this->statedir))) {
                return $this->raiseError("could not create directory '{$this->statedir}'");
            }
        }
        return true;
    }

    // }}}
    // {{{ _packageFileName()

    /**
     * Get the name of the file where data for a given package is stored.
     *
     * @param string package name
     *
     * @return string registry file name
     *
     * @access public
     */
    function _packageFileName($package)
    {
        return $this->statedir . DIRECTORY_SEPARATOR . strtolower($package) . '.reg';
    }

    // }}}
    // {{{ _openPackageFile()

    function _openPackageFile($package, $mode)
    {
        $this->_assertStateDir();
        $file = $this->_packageFileName($package);
        $fp = @fopen($file, $mode);
        if (!$fp) {
            return null;
        }
        return $fp;
    }

    // }}}
    // {{{ _closePackageFile()

    function _closePackageFile($fp)
    {
        fclose($fp);
    }

    // }}}
    // {{{ rebuildFileMap()

    function rebuildFileMap()
    {
        $packages = $this->listPackages();
        $files = array();
        foreach ($packages as $package) {
            $version = $this->packageInfo($package, 'version');
            $filelist = $this->packageInfo($package, 'filelist');
            if (!is_array($filelist)) {
                continue;
            }
            foreach ($filelist as $name => $attrs) {
                if (isset($attrs['role']) && $attrs['role'] != 'php') {
                    continue;
                }
                if (isset($attrs['baseinstalldir'])) {
                    $file = $attrs['baseinstalldir'].DIRECTORY_SEPARATOR.$name;
                } else {
                    $file = $name;
                }
                $file = preg_replace(',^/+,', '', $file);
                $files[$file] = $package;
            }
        }
        $this->_assertStateDir();
        $fp = @fopen($this->filemap, 'wb');
        if (!$fp) {
            return false;
        }
        $this->filemap_cache = $files;
        fwrite($fp, serialize($files));
        fclose($fp);
        return true;
    }

    // }}}
    // {{{ readFileMap()

    function readFileMap()
    {
        $fp = @fopen($this->filemap, 'r');
        if (!$fp) {
            return $this->raiseError('PEAR_Registry: could not open filemap', PEAR_REGISTRY_ERROR_FILE, null, null, $php_errormsg);
        }
        $fsize = filesize($this->filemap);
        $data = fread($fp, $fsize);
        fclose($fp);
        $tmp = unserialize($data);
        if (!$tmp && $fsize > 7) {
            return $this->raiseError('PEAR_Registry: invalid filemap data', PEAR_REGISTRY_ERROR_FORMAT, null, null, $data);
        }
        $this->filemap_cache = $tmp;
        return true;
    }

    // }}}
    // {{{ _lock()

    /**
     * Lock the registry.
     *
     * @param integer lock mode, one of LOCK_EX, LOCK_SH or LOCK_UN.
     *                See flock manual for more information.
     *
     * @return bool TRUE on success, FALSE if locking failed, or a
     *              PEAR error if some other error occurs (such as the
     *              lock file not being writable).
     *
     * @access private
     */
    function _lock($mode = LOCK_EX)
    {
        if (!eregi('Windows 9', php_uname())) {
            if ($mode != LOCK_UN && is_resource($this->lock_fp)) {
                // XXX does not check type of lock (LOCK_SH/LOCK_EX)
                return true;
            }
            if (PEAR::isError($err = $this->_assertStateDir())) {
                return $err;
            }
            $open_mode = 'w';
            // XXX People reported problems with LOCK_SH and 'w'
            if ($mode === LOCK_SH) {
                if (@!is_file($this->lockfile)) {
                    touch($this->lockfile);
                }
                $open_mode = 'r';
            }

            $this->lock_fp = @fopen($this->lockfile, $open_mode);

            if (!is_resource($this->lock_fp)) {
                return $this->raiseError("could not create lock file" .
                                         (isset($php_errormsg) ? ": " . $php_errormsg : ""));
            }
            if (!(int)flock($this->lock_fp, $mode)) {
                switch ($mode) {
                    case LOCK_SH: $str = 'shared';    break;
                    case LOCK_EX: $str = 'exclusive'; break;
                    case LOCK_UN: $str = 'unlock';    break;
                    default:      $str = 'unknown';   break;
                }
                return $this->raiseError("could not acquire $str lock ($this->lockfile)",
                                         PEAR_REGISTRY_ERROR_LOCK);
            }
        }
        return true;
    }

    // }}}
    // {{{ _unlock()

    function _unlock()
    {
        $ret = $this->_lock(LOCK_UN);
        $this->lock_fp = null;
        return $ret;
    }

    // }}}
    // {{{ _packageExists()

    function _packageExists($package)
    {
        return file_exists($this->_packageFileName($package));
    }

    // }}}
    // {{{ _packageInfo()

    function _packageInfo($package = null, $key = null)
    {
        if ($package === null) {
            return array_map(array($this, '_packageInfo'),
                             $this->_listPackages());
        }
        $fp = $this->_openPackageFile($package, 'r');
        if ($fp === null) {
            return null;
        }
        $data = fread($fp, filesize($this->_packageFileName($package)));
        $this->_closePackageFile($fp);
        $data = unserialize($data);
        if ($key === null) {
            return $data;
        }
        if (isset($data[$key])) {
            return $data[$key];
        }
        return null;
    }

    // }}}
    // {{{ _listPackages()

    function _listPackages()
    {
        $pkglist = array();
        $dp = @opendir($this->statedir);
        if (!$dp) {
            return $pkglist;
        }
        while ($ent = readdir($dp)) {
            if ($ent{0} == '.' || substr($ent, -4) != '.reg') {
                continue;
            }
            $pkglist[] = substr($ent, 0, -4);
        }
        return $pkglist;
    }

    // }}}

    // {{{ packageExists()

    function packageExists($package)
    {
        if (PEAR::isError($e = $this->_lock(LOCK_SH))) {
            return $e;
        }
        $ret = $this->_packageExists($package);
        $this->_unlock();
        return $ret;
    }

    // }}}
    // {{{ packageInfo()

    function packageInfo($package = null, $key = null)
    {
        if (PEAR::isError($e = $this->_lock(LOCK_SH))) {
            return $e;
        }
        $ret = $this->_packageInfo($package, $key);
        $this->_unlock();
        return $ret;
    }

    // }}}
    // {{{ listPackages()

    function listPackages()
    {
        if (PEAR::isError($e = $this->_lock(LOCK_SH))) {
            return $e;
        }
        $ret = $this->_listPackages();
        $this->_unlock();
        return $ret;
    }

    // }}}
    // {{{ addPackage()

    function addPackage($package, $info)
    {
        if ($this->packageExists($package)) {
            return false;
        }
        if (PEAR::isError($e = $this->_lock(LOCK_EX))) {
            return $e;
        }
        $fp = $this->_openPackageFile($package, 'wb');
        if ($fp === null) {
            $this->_unlock();
            return false;
        }
        $info['_lastmodified'] = time();
        fwrite($fp, serialize($info));
        $this->_closePackageFile($fp);
        $this->_unlock();
        return true;
    }

    // }}}
    // {{{ deletePackage()

    function deletePackage($package)
    {
        if (PEAR::isError($e = $this->_lock(LOCK_EX))) {
            return $e;
        }
        $file = $this->_packageFileName($package);
        $ret = @unlink($file);
        $this->rebuildFileMap();
        $this->_unlock();
        return $ret;
    }

    // }}}
    // {{{ updatePackage()

    function updatePackage($package, $info, $merge = true)
    {
        $oldinfo = $this->packageInfo($package);
        if (empty($oldinfo)) {
            return false;
        }
        if (PEAR::isError($e = $this->_lock(LOCK_EX))) {
            return $e;
        }
        $fp = $this->_openPackageFile($package, 'w');
        if ($fp === null) {
            $this->_unlock();
            return false;
        }
        $info['_lastmodified'] = time();
        if ($merge) {
            fwrite($fp, serialize(array_merge($oldinfo, $info)));
        } else {
            fwrite($fp, serialize($info));
        }
        $this->_closePackageFile($fp);
        if (isset($info['filelist'])) {
            $this->rebuildFileMap();
        }
        $this->_unlock();
        return true;
    }

    // }}}
    // {{{ checkFileMap()

    /**
     * Test whether a file belongs to a package.
     *
     * @param string $path file path, absolute or relative to the pear
     * install dir
     *
     * @return string which package the file belongs to, or an empty
     * string if the file does not belong to an installed package
     *
     * @access public
     */
    function checkFileMap($path)
    {
        if (is_array($path)) {
            static $notempty;
            if (empty($notempty)) {
                $notempty = create_function('$a','return !empty($a);');
            }
            $pkgs = array();
            foreach ($path as $name => $attrs) {
                if (is_array($attrs) && isset($attrs['baseinstalldir'])) {
                    $name = $attrs['baseinstalldir'].DIRECTORY_SEPARATOR.$name;
                }
                $pkgs[$name] = $this->checkFileMap($name);
            }
            return array_filter($pkgs, $notempty);
        }
        if (empty($this->filemap_cache) && PEAR::isError($this->readFileMap())) {
            return $err;
        }
        if (isset($this->filemap_cache[$path])) {
            return $this->filemap_cache[$path];
        }
        $l = strlen($this->install_dir);
        if (substr($path, 0, $l) == $this->install_dir) {
            $path = preg_replace('!^'.DIRECTORY_SEPARATOR.'+!', '', substr($path, $l));
        }
        if (isset($this->filemap_cache[$path])) {
            return $this->filemap_cache[$path];
        }
        return '';
    }

    // }}}

}

?>
