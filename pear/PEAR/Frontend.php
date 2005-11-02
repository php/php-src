<?php
/**
 * PEAR_Frontend, the singleton-based frontend for user input/output
 *
 * PHP versions 4 and 5
 *
 * LICENSE: This source file is subject to version 3.0 of the PHP license
 * that is available through the world-wide-web at the following URI:
 * http://www.php.net/license/3_0.txt.  If you did not receive a copy of
 * the PHP License and are unable to obtain it through the web, please
 * send a note to license@php.net so we can mail you a copy immediately.
 *
 * @category   pear
 * @package    PEAR
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    CVS: $Id$
 * @link       http://pear.php.net/package/PEAR
 * @since      File available since Release 1.4.0a1
 */

/**
 * Which user interface class is being used.
 * @var string class name
 */
$GLOBALS['_PEAR_FRONTEND_CLASS'] = 'PEAR_Frontend_CLI';

/**
 * Instance of $_PEAR_Command_uiclass.
 * @var object
 */
$GLOBALS['_PEAR_FRONTEND_SINGLETON'] = null;

/**
 * Singleton-based frontend for PEAR user input/output
 * @category   pear
 * @package    PEAR
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    Release: @package_version@
 * @link       http://pear.php.net/package/PEAR
 * @since      Class available since Release 1.4.0a1
 */
class PEAR_Frontend extends PEAR
{
    /**
     * Retrieve the frontend object
     * @return PEAR_Frontend_CLI|PEAR_Frontend_Web|PEAR_Frontend_Gtk
     * @static
     */
    function &singleton($type = null)
    {
        if ($type === null) {
            if (!isset($GLOBALS['_PEAR_FRONTEND_SINGLETON'])) {
                $a = false;
                return $a;
            }
            return $GLOBALS['_PEAR_FRONTEND_SINGLETON'];
        } else {
            $a = PEAR_Frontend::setFrontendClass($type);
            return $a;
        }
    }

    function &setFrontendClass($uiclass)
    {
        if (is_object($GLOBALS['_PEAR_FRONTEND_SINGLETON']) &&
              is_a($GLOBALS['_PEAR_FRONTEND_SINGLETON'], $uiclass)) {
            return $GLOBALS['_PEAR_FRONTEND_SINGLETON'];
        }
        if (!class_exists($uiclass)) {
            $file = str_replace('_', '/', $uiclass) . '.php';
            if (PEAR_Frontend::isIncludeable($file)) {
                include_once $file;
            }
        }
        if (class_exists($uiclass)) {
            $obj = &new $uiclass;
            // quick test to see if this class implements a few of the most
            // important frontend methods
            if (method_exists($obj, 'userConfirm')) {
                $GLOBALS['_PEAR_FRONTEND_SINGLETON'] = &$obj;
                $GLOBALS['_PEAR_FRONTEND_CLASS'] = $uiclass;
                return $obj;
            } else {
                $err = PEAR::raiseError("not a frontend class: $uiclass");
                return $err;
            }
        }
        $err = PEAR::raiseError("no such class: $uiclass");
        return $err;
    }

    /**
     * @param string $path relative or absolute include path
     * @return boolean
     * @static
     */
    function isIncludeable($path)
    {
        if (file_exists($path) && is_readable($path)) {
            return true;
        }
        $ipath = explode(PATH_SEPARATOR, ini_get('include_path'));
        foreach ($ipath as $include) {
            $test = realpath($include . DIRECTORY_SEPARATOR . $path);
            if (!$test) { // support wrappers like phar (realpath just don't work with them)
                $test = $include . DIRECTORY_SEPARATOR . $path;
            }
            if (file_exists($test) && is_readable($test)) {
                return true;
            }
        }
        return false;
    }

    /**
     * @param PEAR_Config
     */
    function setConfig(&$config)
    {
    }

    /**
     * This can be overridden to allow session-based temporary file management
     *
     * By default, all files are deleted at the end of a session.  The web installer
     * needs to be able to sustain a list over many sessions in order to support
     * user interaction with install scripts
     */
    function addTempFile($file)
    {
        $GLOBALS['_PEAR_Common_tempfiles'][] = $file;
    }

    function log($level, $msg, $append_crlf = true)
    {
    }
}
?>