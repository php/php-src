--TEST--
Test get_defined_constants() function : basic functionality 
--FILE--
<?php
/* Prototype  : array get_defined_constants  ([ bool $categorize  ] )
 * Description:  Returns an associative array with the names of all the constants and their values
 * Source code: Zend/zend_builtin_functions.c
 */		

echo "*** Testing get_defined_constants() : basic functionality ***\n";

var_dump(gettype(get_defined_constants(true)));
var_dump(gettype(get_defined_constants()));

$arr1 = get_defined_constants(false);
$arr2 = get_defined_constants();
var_dump(array_diff($arr1, $arr2));

$n1 = count(get_defined_constants());
define("USER_CONSTANT", "test");
$arr2 = get_defined_constants();
$n2 = count($arr2);

if ($n2 == $n1 + 1 && array_key_exists("USER_CONSTANT", $arr2)) {
	echo "TEST PASSED\n";
} else {
	echo "TEST FAILED\n";
}

?>
===DONE===
--EXPECTF-- 
*** Testing get_defined_constants() : basic functionality ***
string(5) "array"
string(5) "array"
array(0) {
}
TEST PASSED
===DONE===