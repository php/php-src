--TEST--
Test property_exists() function : class auto loading
--FILE--
<?php
/* Prototype  : bool property_exists(mixed object_or_class, string property_name)
 * Description: Checks if the object or class has a property
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions:
 */

echo "*** Testing property_exists() : class auto loading ***\n";

spl_autoload_register(function ($class_name) {
    require_once $class_name . '.inc';
});

echo "\ntesting property in autoloaded class\n";
var_dump(property_exists("AutoTest", "bob"));

echo "\ntesting __get magic method\n";
var_dump(property_exists("AutoTest", "foo"));

?>
===DONE===
--EXPECTF--
*** Testing property_exists() : class auto loading ***

testing property in autoloaded class
bool(true)

testing __get magic method
bool(false)
===DONE===
