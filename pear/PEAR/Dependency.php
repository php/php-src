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

require_once "PEAR.php";

class PEAR_Dependency
{
    /**
     * Package dependencies check method
     *
     * @param string $name      Name of the package to test
     * @param string $version   The package version required
     * @param string $relation  How to compare versions with eachother
     *
     * @return bool whether the dependency is satisfied
     */
    function checkPackage($name, $req_ver = null, $relation = 'has')
    {
        if (empty($this->registry)) {
            $this->registry = new PEAR_Registry;
        }
        $pkg_ver = $this->registry->packageInfo($name, 'version');
        if ($relation == 'has') {
            return $this->registry->packageExists($name);
        }
        if (substr($relation, 0, 2) == 'v.') {
            $operator = substr($relation, 2);
            return version_compare($req_ver, $pkg_ver, $operator);
        }
        return false;
    }

    /**
     * Extension dependencies check method
     *
     * @param string $name        Name of the extension to test
     * @param string $req_ext_ver Required extension version to compare with
     * @param string $relation    How to compare versions with eachother
     *
     * @return bool whether the dependency is satisfied
     */
    function checkExtension($name, $req_ver = null, $relation = 'has')
    {
        // XXX (ssb): could we avoid loading the extension here?
        if (!extension_loaded($name)) {
            $dlext = OS_WINDOWS ? '.dll' : '.so';
            if (!@dl($name . $dlext)) {
                return false;
            }
        }
        $ext_ver = phpversion($name);
        if ($relation == 'has') {
            return true;
        }
        if (substr($relation, 0, 2) == 'v.') {
            $operator = substr($relation, 2);
            return version_compare($req_ver, $ext_ver, $operator);
        }
        return false;
    }


    /**
     * Operating system  dependencies check method
     *
     * @param string $os  Name of the operating system
     *
     * @return bool whether we're running on $os
     */
    function checkOS($os)
    {
        // only 'has' relation is supported
        return ($os == PHP_OS);
    }

    /**
     * PHP version check method
     *
     * @param string $req_ver   which version to compare
     * @param string $relation  how to compare the version
     *
     * @return bool whether the dependency is satisfied
     */
    function checkPHP($req_ver, $relation = 'ge')
    {
        $php_ver = phpversion();
        if (substr($relation, 0, 2) == 'v.') {
            $operator = substr($relation, 2);
            return version_compare($req_ver, $php_ver, $operator);
        }
        return true;
    }

    /**
     * External program check method.  Looks for executable files in
     * directories listed in the PATH environment variable.
     *
     * @param string $program   which program to look for
     *
     * @return bool whether the dependency is satisfied
     */
    function checkProgram($program)
    {
        // XXX FIXME honor safe mode
        $path_delim = OS_WINDOWS ? ';' : ':';
        $exe_suffix = OS_WINDOWS ? '.exe' : '';
        $path_elements = explode($path_delim, $_ENV['PATH']);
        foreach ($path_elements as $dir) {
            $file = "$dir/$program{$exe_suffix}";
            if (file_exists($file) && is_executable($file)) {
                return true;
            }
        }
        return false;
    }

    /**
     * SAPI backend check method.  Version comparison is not yet
     * available here.
     *
     * @param string $name      name of SAPI backend
     * @param string $req_ver   which version to compare
     * @param string $relation  how to compare versions (currently
     *                          hardcoded to 'has')
     */
    function checkSAPI($name, $req_ver = null, $relation = 'has')
    {
        $sapi_backend = php_sapi_name();
        // Version comparisons not supported, sapi backends don't have
        // version information yet.
        return ($sapi_backend == $name);
    }
}

?>