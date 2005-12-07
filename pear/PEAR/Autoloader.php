<?php
// /* vim: set expandtab tabstop=4 shiftwidth=4: */
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
// | Author: Stig Bakken <ssb@php.net>                                    |
// |                                                                      |
// +----------------------------------------------------------------------+
//
// $Id$

if (!extension_loaded("overload")) {
    // die hard without ext/overload
    die("Rebuild PHP with the `overload' extension to use PEAR_Autoloader");
}

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
 * @author Stig Sæther Bakken <ssb@php.net>
 */
class PEAR_Autoloader extends PEAR
{
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
            $this->_autoload_map = array_merge($this->_autoload_map, $method);
        } else {
            $this->_autoload_map[$method] = $classname;
        }
    }

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
        $ok = isset($this->_autoload_map[$method]);
        unset($this->_autoload_map[$method]);
        return $ok;
    }

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
            if (get_class($obj) == $classname) {
                unset($this->_method_map[$method]);
                $ok = true;
            }
        }
        return $ok;
    }

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
        if (empty($this->_method_map[$method]) && isset($this->_autoload_map[$method])) {
            $this->addAggregateObject($this->_autoload_map[$method]);
        }
        if (isset($this->_method_map[$method])) {
            $retval = call_user_func_array(array($this->_method_map[$method], $method), $args);
            return true;
        }
        return false;
    }
}

overload("PEAR_Autoloader");

?>
