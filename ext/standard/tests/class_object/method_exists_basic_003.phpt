--TEST--
method_exists() on non-existent class, with __autoload().
--FILE--
<?php
/* Prototype  : proto bool is_subclass_of(object object, string class_name)
 * Description: Returns true if the object has this class as one of its parents
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions:
 */

spl_autoload_register(function ($name) {
	echo "In autoload($name)\n";
});

var_dump(method_exists('UndefC', 'func'));

echo "Done";
?>
--EXPECT--
In autoload(UndefC)
bool(false)
Done
