<?php
/**
 * Class auto-loader
 *
 * PHP versions 4
 *
 * LICENSE: This source file is subject to version 3.0 of the PHP license
 * that is available through the world-wide-web at the following URI:
 * http://www.php.net/license/3_0.txt.  If you did not receive a copy of
 * the PHP License and are unable to obtain it through the web, please
 * send a note to license@php.net so we can mail you a copy immediately.
 *
 * @category   pear
 * @package    PEAR
 * @author     Stig Bakken <ssb@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    CVS: $Id$
 * @link       http://pear.php.net/manual/en/core.ppm.php#core.ppm.pear-autoloader
 * @since      File available since Release 0.1
 * @deprecated File deprecated in Release 1.4.0a1
 */

// /* vim: set expandtab tabstop=4 shiftwidth=4: */

if (!extension_loaded("overload")) {
    // die hard without ext/overload
    die("Rebuild PHP with the `overload' extension to use PEAR_Autoloader");
}

/**
 * Include for PEAR_Error and PEAR classes
 */
require_once "PEAR.php";

/**
 * This class is for objects where you want to separate the code for
 * some methods into separate classes.  This is useful if you have a
 * class with not-frequently-used methods that contain lots of code
 * that you would like to avoid always parsing.
 *
 * The PEAR_Autoloader class provides autoloading and aggregation.
 * The autoloading lets you set up in which classes the separated
 * methods are found.  Aggregation is the technique used to import new
 * methods, an instance of each class providing separated methods is
 * stored and called every time the aggregated method is called.
 *
 * @category   pear
 * @package    PEAR
 * @author Stig Bakken <ssb@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    Release: @package_version@
 * @link       http://pear.php.net/manual/en/core.ppm.php#core.ppm.pear-autoloader
 * @since      File available since Release 0.1
 * @deprecated File deprecated in Release 1.4.0a1
 */
class PEAR_Autoloader extends PEAR
{
    // {{{ properties

    /**
     * Map of methods and classes where they are defined
     *
     * @var array
     *
     * @access private
     */
    var $_autoload_map = array();

    /**
     * Map of methods and aggregate objects
     *
     * @var array
     *
     * @access private
     */
    var $_method_map = array();

    // }}}
    // {{{ addAutoload()

    /**
     * Add one or more autoload entries.
     *
     * @param string $method     which method to autoload
     *
     * @param string $classname  (optional) which class to find the method in.
     *                           If the $method parameter is an array, this
     *                           parameter may be omitted (and will be ignored
     *                           if not), and the $method parameter will be
     *                           treated as an associative array with method
     *                           names as keys and class names as values.
     *
     * @return void
     *
     * @access public
     */
    function addAutoload($method, $classname = null)
    {
        if (is_array($method)) {
            array_walk($method, create_function('$a,&$b', '$b = strtolower($b);'));
            $this->_autoload_map = array_merge($this->_autoload_map, $method);
        } else {
            $this->_autoload_map[strtolower($method)] = $classname;
        }
    }

    // }}}
    // {{{ removeAutoload()

    /**
     * Remove an autoload entry.
     *
     * @param string $method  which method to remove the autoload entry for
     *
     * @return bool TRUE if an entry was removed, FALSE if not
     *
     * @access public
     */
    function removeAutoload($method)
    {
        $method = strtolower($method);
        $ok = isset($this->_autoload_map[$method]);
        unset($this->_autoload_map[$method]);
        return $ok;
    }

    // }}}
    // {{{ addAggregateObject()

    /**
     * Add an aggregate object to this object.  If the specified class
     * is not defined, loading it will be attempted following PEAR's
     * file naming scheme.  All the methods in the class will be
     * aggregated, except private ones (name starting with an
     * underscore) and constructors.
     *
     * @param string $classname  what class to instantiate for the object.
     *
     * @return void
     *
     * @access public
     */
    function addAggregateObject($classname)
    {
        $classname = strtolower($classname);
        if (!class_exists($classname)) {
            $include_file = preg_replace('/[^a-z0-9]/i', '_', $classname);
            include_once $include_file;
        }
        $obj =& new $classname;
        $methods = get_class_methods($classname);
        foreach ($methods as $method) {
            // don't import priviate methods and constructors
            if ($method{0} != '_' && $method != $classname) {
                $this->_method_map[$method] = $obj;
            }
        }
    }

    // }}}
    // {{{ removeAggregateObject()

    /**
     * Remove an aggregate object.
     *
     * @param string $classname  the class of the object to remove
     *
     * @return bool  TRUE if an object was removed, FALSE if not
     *
     * @access public
     */
    function removeAggregateObject($classname)
    {
        $ok = false;
        $classname = strtolower($classname);
        reset($this->_method_map);
        while (list($method, $obj) = each($this->_method_map)) {
            if (is_a($obj, $classname)) {
                unset($this->_method_map[$method]);
                $ok = true;
            }
        }
        return $ok;
    }

    // }}}
    // {{{ __call()

    /**
     * Overloaded object call handler, called each time an
     * undefined/aggregated method is invoked.  This method repeats
     * the call in the right aggregate object and passes on the return
     * value.
     *
     * @param string $method  which method that was called
     *
     * @param string $args    An array of the parameters passed in the
     *                        original call
     *
     * @return mixed  The return value from the aggregated method, or a PEAR
     *                error if the called method was unknown.
     */
    function __call($method, $args, &$retval)
    {
        $method = strtolower($method);
        if (empty($this->_method_map[$method]) && isset($this->_autoload_map[$method])) {
            $this->addAggregateObject($this->_autoload_map[$method]);
        }
        if (isset($this->_method_map[$method])) {
            $retval = call_user_func_array(array($this->_method_map[$method], $method), $args);
            return true;
        }
        return false;
    }

    // }}}
}

overload("PEAR_Autoloader");

?>
