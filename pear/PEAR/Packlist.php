<?php
//
// +----------------------------------------------------------------------+
// | PHP version 4.0                                                      |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2001 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.02 of the PHP license,      |
// | that is bundled with this package in the file LICENSE, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/2_02.txt.                                 |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors: Stig Bakken <ssb@fast.no>                                   |
// |                                                                      |
// +----------------------------------------------------------------------+
//
// $Id$

require_once "PEAR/Common.php";

/**
 * Administration class used to maintain the installed package database.
 *
 * THIS CLASS IS EXPERIMENTAL
 */
class PEAR_Packlist
{
    /** directory where system state information goes */
    var $statedir;

    /** list of installed packages */
    var $pkglist = array();

    /** temporary directory */
    var $tmpdir;

    /** file pointer for package list file if open */
    var $pkglist_fp;

    // {{{ constructor

    function PEAR_Packlist($phpdir = PEAR_INSTALL_DIR,
                            $extdir = PEAR_EXTENSION_DIR,
                            $docdir = '')
    {
        $this->PEAR();
        $this->phpdir = $phpdir;
        $this->extdir = $extdir;
        $this->docdir = $docdir;
        $this->statedir = "/var/lib/php"; // XXX FIXME Windows
    }

    // }}}
    // {{{ destructor

    function _PEAR_Packlist()
    {
        if ($this->tmpdir && is_dir($this->tmpdir)) {
            system("rm -rf $this->tmpdir"); // XXX FIXME Windows
        }
        if (isset($this->pkglist_fp) && is_resource($this->pkglist_fp)) {
            flock($this->pkglist_fp, LOCK_UN);
            fclose($this->pkglist_fp);
        }
        $this->tmpdir = null;
        $this->pkglist_fp = null;
        $this->_PEAR_Common();
    }

    // }}}

    // {{{ lockPackageList()

    function lockPackageList()
    {
        $fp = $this->pkglist_fp;
        if (!is_resource($fp)) {
            $this->pkglist_fp = $fp = fopen($this->pkglist_file, "r");
        }
        return flock($fp, LOCK_EX);
    }

    // }}}
    // {{{ unlockPackageList()

    function unlockPackageList()
    {
        $fp = $this->pkglist_fp;
        if (!is_resource($fp)) {
            $this->pkglist_fp = $fp = fopen($this->pkglist_file, "r");
            $doclose = true;
        }
        $ret = flock($fp, LOCK_EX);
        if ($doclose) {
            fclose($fp);
        }
        return $ret;
    }

    // }}}
    // {{{ loadPackageList()

    function loadPackageList($file)
    {
        $this->pkglist_file = $file;
        $this->pkglist = array();
        if (!file_exists($file)) {
            if (!@touch($file)) {
                    return $this->raiseError("touch($file): $php_errormsg");
            }
        }
        $fp = $this->pkglist_fp = @fopen($file, "r");
        if (!is_resource($fp)) {
                return $this->raiseError("fopen($file): $php_errormsg");
        }
        $this->lockPackageList();
        $versionline = trim(fgets($fp, 2048));
        if ($versionline == ";1") {
            while ($line = fgets($fp, 2048)) {
                list($name, $version, $file) = explode(";", trim($line));
                $this->pkglist[$name]["version"] = $version;
                $this->pkglist[$name]["files"][] = $file;
            }
        }
        $this->unlockPackageList();
    }

    // }}}
    // {{{ savePackageList()

    function savePackageList()
    {
        $fp = $this->pkglist_fp;
        $wfp = @fopen($this->pkglist_file, "w");
        if (!is_resource($wfp)) {
            return $this->raiseError("could not write $this->pkglist_file");
        }
        if (is_resource($fp)) {
            fclose($fp);
        }
        $this->pkglist_fp = $fp = $wfp;
        fwrite($fp, ";1\n");
        foreach ($this->pkglist as $name => $entry) {
            $ver = $entry["version"];
            foreach ($entry["files"] as $file) {
                fwrite($fp, "$name;$ver;$file\n");
            }
        }
        fclose($fp);
        $this->pkglist_fp = $fp = null;
    }

    // }}}
    // {{{ updatePackageListFrom()

    function updatePackageListFrom($file)
    {
        /*
    $new = $this->classesDeclaredBy($file);
    reset($new);
    while (list($i, $name) = each($new)) {
        $this->pkglist['class'][$name] = $file;
    }
        */
    }

    // }}}
    // {{{ classesDeclaredBy()

    /**
     * Find out which new classes are defined by a file.
     *
     * @param $file file name passed to "include"
     *
     * @return array classes that were defined
     */
    function classesDeclaredBy($file)
    {
        $before = get_declared_classes();
        ob_start();
        include($file);
        ob_end_clean();
        $after = get_declared_classes();
        // using array_slice to renumber array
        $diff = array_slice(array_diff($after, $before), 0);
        return $diff;
    }

    // }}}

    // {{{ declaredWhenIncluding()

    /**
     * Find out which new classes are defined by a file.
     *
     * @param $file file name passed to "include"
     *
     * @return array classes that were defined
     */
    function &declaredWhenIncluding($file)
    {
        $classes_before = get_declared_classes();
        $funcs_before = get_defined_functions();
//      $vars_before = $GLOBALS;
        ob_start();
        include($file);
        ob_end_clean();
        $classes_after = get_declared_classes();
        $funcs_after = get_defined_functions();
//      $vars_after = $GLOBALS;
        //using array_slice to renumber array
        return array(
            "classes" => array_slice(array_diff($classes_after, $classes_before), 0),
            "functions" => array_slice(array_diff($funcs_after, $funcs_before), 0),
//          "globals" => array_slice(array_diff($vars_after, $vars_before), 0)
        );
    }

    // }}}

    // {{{ lockDir()

    /**
     * Uses advisory locking (flock) to temporarily claim $dir as its
     * own.
     *
     * @param $dir the directory to lock
     *
     * @return bool true if successful, false if not
     */
    function lockDir($dir)
    {
        $lockfile = "$dir/.lock";
        if (!file_exists($lockfile)) {
            if (!touch($lockfile)) {
            // could not create lockfile!
            return false;
            }
        }
        $fp = fopen($lockfile, "r");
        if (!flock($fp, LOCK_EX)) {
            // could not create lock!
            return false;
        }
        return true;
    }

    // }}}

}
?>