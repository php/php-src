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

require_once "Experimental/System.php";

/**
 * Administration class used to maintain the installed package database.
 */
class PEAR_Registry
{
    // {{{ properties

    var $statedir;

    // }}}

    // {{{ PEAR_Registry

    function PEAR_Registry()
    {
        $this->statedir = PHP_LOCALSTATEDIR . "/pear/registry";
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
        $package = strtolower($package);
        return "{$this->statedir}/{$package}.inf";
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
        return $this->upgradePackage($package, $info);
    }

    // }}}
    // {{{ packageInfo()

    function packageInfo($package)
    {
        $fp = $this->_openPackageFile($package, "r");
        $data = fread($fp, filesize($this->_packageFileName($package)));
        $this->_closePackageFile($fp);
        return unserialize($data);
    }

    // }}}
    // {{{ deletePackage()

    function deletePackage($package)
    {
        $file = $this->_packageFileName($package);
        unlink($file);
    }

    // }}}
    // {{{ upgradePackage()

    function upgradePackage($package, $info)
    {
        $fp = $this->_openPackageFile($package, "w");
        if ($fp === null) {
            return false;
        }
        fwrite($fp, serialize($info));
        $this->_closePackageFile($fp);
        return true;
    }

    // }}}

}

?>