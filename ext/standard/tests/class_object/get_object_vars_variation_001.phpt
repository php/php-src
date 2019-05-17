--TEST--
get_object_vars() - ensure statics are not shown
--FILE--
<?php
/* Prototype  : proto array get_object_vars(object obj)
 * Description: Returns an array of object properties
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions:
 */

Class A {
	public static $var = 'hello';
}

$a = new A;
var_dump(get_object_vars($a));
?>
--EXPECT--
array(0) {
}
