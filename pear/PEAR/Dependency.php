<?php
//
// +----------------------------------------------------------------------+
// | PHP Version 4                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2003 The PHP Group                                |
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
// |          Stig Bakken <ssb@php.net>                                   |
// +----------------------------------------------------------------------+
//
// $Id$

/**
* Methods for dependencies check. Based on Stig's dependencies RFC
* at http://cvs.php.net/cvs.php/pearweb/rfc
* (requires php >= 4.1)
*/

require_once "PEAR.php";

define('PEAR_DEPENDENCY_MISSING',        -1);
define('PEAR_DEPENDENCY_CONFLICT',       -2);
define('PEAR_DEPENDENCY_UPGRADE_MINOR',  -3);
define('PEAR_DEPENDENCY_UPGRADE_MAJOR',  -4);
define('PEAR_DEPENDENCY_BAD_DEPENDENCY', -5);

class PEAR_Dependency
{
    function PEAR_Dependency(&$registry)
    {
        $this->registry = &$registry;
    }
    /**
    * This method maps the xml dependency definition to the
    * PEAR_dependecy one
    *
    * $opts => Array
    *    (
    *        [type] => pkg
    *        [rel] => ge
    *        [version] => 3.4
    *        [name] => HTML_Common
    *    )
    */
    function callCheckMethod(&$errmsg, $opts)
    {
        $rel = isset($opts['rel']) ? $opts['rel'] : 'has';
        $req = isset($opts['version']) ? $opts['version'] : null;
        $name = isset($opts['name']) ? $opts['name'] : null;
        $errmsg = '';
        switch ($opts['type']) {
            case 'pkg':
                return $this->checkPackage($errmsg, $name, $req, $rel);
                break;
            case 'ext':
                return $this->checkExtension($errmsg, $name, $req, $rel);
                break;
            case 'php':
                return $this->checkPHP($errmsg, $req, $rel);
                break;
            case 'prog':
                return $this->checkProgram($errmsg, $name);
                break;
            case 'os':
                return $this->checkOS($errmsg, $name);
                break;
            case 'sapi':
                return $this->checkSAPI($errmsg, $name);
                break;
            case 'zend':
                return $this->checkZend($errmsg, $name);
                break;
            default:
                return "'{$opts['type']}' dependency type not supported";
        }
    }

    /**
     * Package dependencies check method
     *
     * @param string $name      Name of the package to test
     * @param string $version   The package version required
     * @param string $relation  How to compare versions with eachother
     *
     * @return mixed bool false if no error or the error string
     */
    function checkPackage(&$errmsg, $name, $req = null, $relation = 'has')
    {
        if (substr($relation, 0, 2) == 'v.') {
            $relation = substr($relation, 2);
        }
        switch ($relation) {
            case 'has':
                if (!$this->registry->packageExists($name)) {
                    $errmsg = "requires package `$name'";
                    return PEAR_DEPENDENCY_MISSING;
                }
                return false;
            case 'not':
                if (!$this->registry->packageExists($name)) {
                    $errmsg = "conflicts with package `$name'";
                    return PEAR_DEPENDENCY_CONFLICT;
                }
                return false;
            case 'lt':
            case 'le':
            case 'eq':
            case 'ne':
            case 'ge':
            case 'gt':
                $version = $this->registry->packageInfo($name, 'version');
                if (!$this->registry->packageExists($name)
                    || !version_compare("$version", "$req", $relation))
                {
                    $errmsg = "requires package `$name' " .
                        $this->signOperator($relation) . " $req";
                    $code = $this->codeFromRelation($relation, $version, $req);
                }
                return false;
        }
        $errmsg = "relation '$relation' with requirement '$req' is not supported (name=$name)";
        return PEAR_DEPENDENCY_BAD_DEPENDENCY;
    }

    /**
     * Extension dependencies check method
     *
     * @param string $name        Name of the extension to test
     * @param string $req_ext_ver Required extension version to compare with
     * @param string $relation    How to compare versions with eachother
     *
     * @return mixed bool false if no error or the error string
     */
    function checkExtension(&$errmsg, $name, $req = null, $relation = 'has')
    {
        // XXX (ssb): could we avoid loading the extension here?
        if (!PEAR::loadExtension($name)) {
            $errmsg = "'$name' PHP extension is not installed";
            return PEAR_DEPENDENCY_MISSING;
        }
        if ($relation == 'has') {
            return false;
        }
        $code = false;
        if (substr($relation, 0, 2) == 'v.') {
            $ext_ver = phpversion($name);
            $operator = substr($relation, 2);
            // Force params to be strings, otherwise the comparation will fail (ex. 0.9==0.90)
            settype($req, "string");
            if (!version_compare("$ext_ver", "$req", $operator)) {
                $retval = "'$name' PHP extension version " .
                    $this->signOperator($operator) . " $req is required";
                $code = $this->codeFromRelation($relation, $ext_ver, $req);
            }
        }
        return $code;
    }

    /**
     * Operating system  dependencies check method
     *
     * @param string $os  Name of the operating system
     *
     * @return mixed bool false if no error or the error string
     */
    function checkOS(&$errmsg, $os)
    {
        // XXX Fixme: Implement a more flexible way, like
        // comma separated values or something similar to PEAR_OS
        static $myos;
        if (empty($myos)) {
            include_once "OS/Guess.php";
            $myos = new OS_Guess();
        }
        // only 'has' relation is currently supported
        if ($myos->matchSignature($os)) {
            return false;
        }
        $errmsg = "'$os' operating system not supported";
        return PEAR_DEPENDENCY_CONFLICT;
    }

    /**
     * PHP version check method
     *
     * @param string $req   which version to compare
     * @param string $relation  how to compare the version
     *
     * @return mixed bool false if no error or the error string
     */
    function checkPHP(&$errmsg, $req, $relation = 'ge')
    {
        if (substr($relation, 0, 2) == 'v.') {
            $php_ver = phpversion();
            $operator = substr($relation, 2);
            if (!version_compare("$php_ver", "$req", $operator)) {
                $errmsg = "PHP version " . $this->signOperator($operator) .
                    " $req is required";
                return PEAR_DEPENDENCY_CONFLICT;
            }
        }
        return false;
    }

    /**
     * External program check method.  Looks for executable files in
     * directories listed in the PATH environment variable.
     *
     * @param string $program   which program to look for
     *
     * @return mixed bool false if no error or the error string
     */
    function checkProgram(&$errmsg, $program)
    {
        // XXX FIXME honor safe mode
        $path_delim = OS_WINDOWS ? ';' : ':';
        $exe_suffix = OS_WINDOWS ? '.exe' : '';
        $path_elements = explode($path_delim, getenv('PATH'));
        foreach ($path_elements as $dir) {
            $file = $dir . DIRECTORY_SEPARATOR . $program . $exe_suffix;
            if (@file_exists($file) && @is_executable($file)) {
                return false;
            }
        }
        $errmsg = "'$program' program is not present in the PATH";
        return PEAR_DEPENDENCY_MISSING;
    }

    /**
     * SAPI backend check method.  Version comparison is not yet
     * available here.
     *
     * @param string $name      name of SAPI backend
     * @param string $req   which version to compare
     * @param string $relation  how to compare versions (currently
     *                          hardcoded to 'has')
     * @return mixed bool false if no error or the error string
     */
    function checkSAPI(&$errmsg, $name, $req = null, $relation = 'has')
    {
        // XXX Fixme: There is no way to know if the user has or
        // not other SAPI backends installed than the installer one

        $sapi_backend = php_sapi_name();
        // Version comparisons not supported, sapi backends don't have
        // version information yet.
        if ($sapi_backend == $name) {
            return false;
        }
        $errmsg = "'$sapi_backend' SAPI backend not supported";
        return PEAR_DEPENDENCY_CONFLICT;
    }


    /**
     * Zend version check method
     *
     * @param string $req   which version to compare
     * @param string $relation  how to compare the version
     *
     * @return mixed bool false if no error or the error string
     */
    function checkZend(&$errmsg, $req, $relation = 'ge')
    {
        if (substr($relation, 0, 2) == 'v.') {
            $zend_ver = zend_version();
            $operator = substr($relation, 2);
            if (!version_compare("$zend_ver", "$req", $operator)) {
                $errmsg = "Zend version " . $this->signOperator($operator) .
                    " $req is required";
                return PEAR_DEPENDENCY_CONFLICT;
            }
        }
        return false;
    }

    /**
     * Converts text comparing operators to them sign equivalents
     * ex: 'ge' to '>='
     */
    function signOperator($operator)
    {
        switch($operator) {
            case 'lt': return '<';
            case 'le': return '<=';
            case 'gt': return '>';
            case 'ge': return '>=';
            case 'eq': return '==';
            case 'ne': return '!=';
            default:
                return $operator;
        }
    }


    function codeFromRelation($relation, $version, $req)
    {
        $code = PEAR_DEPENDENCY_BAD_DEPENDENCY;
        switch ($relation) {
            case 'gt': case 'ge': case 'eq':
                // upgrade
                $have_major = preg_replace('/\D.*/', '', $version);
                $need_major = preg_replace('/\D.*/', '', $req);
                if ($need_major > $have_major) {
                    $code = PEAR_DEPENDENCY_UPGRADE_MAJOR;
                } else {
                    $code = PEAR_DEPENDENCY_UPGRADE_MINOR;
                }
                break;
            case 'lt': case 'le': case 'ne':
                $code = PEAR_DEPENDENCY_CONFLICT;
                break;
        }
        return $code;
    }
}

?>
