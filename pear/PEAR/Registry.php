<?php
//
// +----------------------------------------------------------------------+
// | PHP Version 4                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2002 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.02 of the PHP license,      |
// | that is bundled with this package in the file LICENSE, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/2_02.txt.                                 |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Author: Stig Bakken <ssb@fast.no>                                    |
// |         Tomas V.V.Cox <cox@idecnet.com>                              |
// |                                                                      |
// +----------------------------------------------------------------------+
//
// $Id$

require_once "System.php";
require_once "PEAR.php";

define("PEAR_REGISTRY_ERROR_LOCK", -2);

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
        $this->statedir = $pear_install_dir.$ds.'.registry';
        $this->filemap  = $pear_install_dir.$ds.'.filemap';
        $this->lockfile = $pear_install_dir.$ds.'.lock';
        if (!file_exists($this->filemap)) {
            $this->_rebuildFileMap();
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
            if (!System::mkdir("-p {$this->statedir}")) {
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
        return "{$this->statedir}/{$package}.reg";
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
    // {{{ _rebuildFileMap()

    function _rebuildFileMap()
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
        $fp = @fopen($this->filemap, 'w');
        if (!$fp) {
            return false;
        }
        fwrite($fp, serialize($files));
        fclose($fp);
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
        if(!strstr(php_uname(), 'Windows 95/98')) {
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

            @ini_set('track_errors', true);
            $this->lock_fp = @fopen($this->lockfile, $open_mode);
            @ini_restore('track_errors');

            if (!is_resource($this->lock_fp)) {
                return $this->raiseError("could not create lock file: $php_errormsg");
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
        $fp = $this->_openPackageFile($package, 'w');
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
        $this->_rebuildFileMap();
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
        if (!file_exists($this->filemap)) {
            $this->_rebuildFileMap();
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
            $this->_rebuildFileMap();
        }
        $this->_unlock();
        return true;
    }

    // }}}

    /**
    Experimental dependencies database handling functions (not yet in production)
    **/

    function rebuildDepsFile()
    {
        $packages = $this->listPackages();
        $files = array();
        foreach ($packages as $package) {
            $deps = $this->packageInfo($package, 'release_deps');
            $this->depUpdatePackage($package, $deps);
        }
        // XXX Change with serialize + write
        return $this->dependencies;
    }
    /*
    The data structure is as follows:
    $dep_db = array(
        // Other packages depends in some manner on this packages
        'deps' => array(
            'Package Name' => array(
                0 => array(
                    // This package depends on 'Package Name'
                    'depend' => 'Package',
                    // Which version 'Package' needs of 'Package Name'
                    'version' => '1.0',
                    // The requirement (version_compare() operator)
                    'rel' => 'ge'
                ),
            ),
        )
        // This packages are dependant on other packages
        'pkgs' => array(
            'Package Dependant' => array(
                0 => array(
                    // This is a index list with paths over the 'deps' array for quick
                    // searching things like "what dependecies has this package?"
                    // $dep_db['deps']['Package Name'][3]
                    0 => 'Package Name',
                    1 => 3 // (int)key
                ),
            ),
        )
    )
    */
    function _addDependency($package, $deps)
    {
        if (!is_array($deps) || !count($deps)) {
            return;
        }
        $data = &$this->dependencies;
        foreach ($deps as $dep) {
            if ($dep && $dep['type'] == 'pkg' && isset($dep['name'])) {
                settype($data['deps'][$dep['name']], 'array');
                $data['deps'][$dep['name']][] = array('depend'  => $package,
                                                      'version' => $dep['version'],
                                                      'rel'     => $dep['rel']);
                $data['pkgs'][$package][] = array($dep['name'], key($data['deps'][$dep['name']]));
            }
        }
    }

    function depRemovePackage($package)
    {
        $data = &$this->dependencies;
        // Other packages depends on this package, can't be removed
        if (isset($data['deps'][$package])) {
            return $data['deps'][$package];
        }
        if (isset($data['pkgs'][$package])) {
            foreach ($data['pkgs'][$package] as $remove) {
                // remove the dependency
                unset($data['deps'][$remove[0]][$remove[1]]);
                // if no more dependencies, remove the subject too
                if (!count($data['deps'][$remove[0]])) {
                    unset($data['deps'][$remove[0]]);
                }
            }
            // remove the package from the index list
            unset($data['pkgs'][$package]);
        }
        return $data;
    }

    function depUpdatePackage($package, $new_version, $release_deps)
    {
        $data = &$this->dependencies;
        // Other packages depend on this package, check deps
        if (isset($data['deps'][$package])) {
            foreach ($data['deps'][$package] as $dep) {
                $require  = $dep['version'];
                $relation = $dep['rel'];
                if ($relation != 'has') {
                    if (!version_compare($new_version, $require, $relation)) {
                        $fails[] = $dep;
                    }
                }
            }
            if (isset($fails)) {
                return $fails;
            }
        }
        $this->_addDependency($package, $release_deps);
    }
}

?>