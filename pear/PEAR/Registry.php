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
// | Authors: Stig Bakken <ssb@fast.no>                                   |
// |                                                                      |
// +----------------------------------------------------------------------+
//
// $Id$

require_once "System.php";

/**
 * Administration class used to maintain the installed package database.
 */
class PEAR_Registry
{
    // {{{ properties

    var $statedir;

    // }}}

    // {{{ PEAR_Registry

    function PEAR_Registry($pear_install_dir = PEAR_INSTALL_DIR)
    {
        $this->statedir = $pear_install_dir . "/.registry";
    }

    // }}}

    // {{{ _assertStateDir()

    function _assertStateDir()
    {
        if (!@is_dir($this->statedir)) {
            System::mkdir("-p {$this->statedir}");
        }
    }

    // }}}
    // {{{ _packageFileName()

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

    // {{{ packageExists()

    function packageExists($package)
    {
        return file_exists($this->_packageFileName($package));
    }

    // }}}
    // {{{ addPackage()

    function addPackage($package, $info)
    {
        if ($this->packageExists($package)) {
            return false;
        }
        $fp = $this->_openPackageFile($package, "w");
        if ($fp === null) {
            return false;
        }
        fwrite($fp, serialize($info));
        $this->_closePackageFile($fp);
        return true;
    }

    // }}}
    // {{{ packageInfo()

    function packageInfo($package = null, $key = null)
    {
        if ($package === null) {
            return array_map(array($this, "packageInfo"),
                             $this->listPackages());
        }
        $fp = $this->_openPackageFile($package, "r");
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
    // {{{ deletePackage()

    function deletePackage($package)
    {
        $file = $this->_packageFileName($package);
        return @unlink($file);
    }

    // }}}
    // {{{ updatePackage()

    function updatePackage($package, $info, $merge = true)
    {
        $oldinfo = $this->packageInfo($package);
        if (empty($oldinfo)) {
            return false;
        }
        $fp = $this->_openPackageFile($package, "w");
        if ($fp === null) {
            return false;
        }
        if ($merge) {
            fwrite($fp, serialize(array_merge($oldinfo, $info)));
        } else {
            fwrite($fp, serialize($info));
        }
        $this->_closePackageFile($fp);
        return true;
    }

    // }}}
    // {{{ listPackages()

    function listPackages()
    {
        $pkglist = array();
        $dp = @opendir($this->statedir);
        if (!$dp) {
            return $pkglist;
        }
        while ($ent = readdir($dp)) {
            if ($ent{0} == "." || substr($ent, -4) != ".reg") {
                continue;
            }
            $pkglist[] = substr($ent, 0, -4);
        }
        return $pkglist;
    }

    // }}}
}

?>