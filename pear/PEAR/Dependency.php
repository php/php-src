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
// | Authors: Tomas V.V.Cox <cox@idecnet.com>                             |
// |          Stig Bakken <ssb@fast.no>                                   |
// |                                                                      |
// +----------------------------------------------------------------------+
//
// $Id$

/**
* Methods for dependencies check. Based on Stig's dependencies RFC
* at http://cvs.php.net/cvs.php/pearweb/rfc
* (requires php >= 4.1)
*/

class PEAR_Dependency
{
    /**
    * Package dependencies check method
    *
    * @param string $name    Name of the package to test
    * @param string $version The package version required
    * @param string $rule    A valid version_compare compare operator
    *
    * @return bool
    */
    function package($name, $version = null, $rule = 'has')
    {
        if (empty($this->registry)) {
            $this->registry = new PEAR_Registry;
        }
        switch ($rule) {
            case 'has':
                return $this->registry->packageExists($name);
                break;
            default:
                if ($info = $this->registry->packageInfo($name)) {
                    return version_compare($version,
                                           $info['version'],
                                           $rule);
                }
                return false;
                break;
        }
    }

    /**
    * Extension dependencies check method
    *
    * @param string $name Name of the extension to test
    * @param string $req_ext_ver Required extension version to compare with
    * @param string $rule A valid version_compare compare operator
    *
    * @return bool
    */
    function extension($name, $req_ext_ver = null, $rule = 'has')
    {
        if (!extension_loaded($name)) {
            $dlext = OS_WINDOWS ? '.dll' : '.so';
            if (!@dl($name . $dlext)) {
                return false;
            } elseif($rule == 'has') {
                return true;
            }
        }
        // XXX Put here the code to extract the version of a extension. Stig? :)
        //$ext_ver = ?????($name);
        return version_compare($req_ext_ver, $ext_ver, $rule);
    }

}

?>