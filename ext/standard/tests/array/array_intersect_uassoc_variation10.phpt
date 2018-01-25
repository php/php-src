--TEST--
Test array_intersect_uassoc() function : usage variation - Passing class/object methods to callback
--FILE--
<?php
/* Prototype  : array array_intersect_uassoc(array arr1, array arr2 [, array ...], callback key_compare_func)
 * Description: Computes the intersection of arrays with additional index check, compares indexes by a callback function
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_intersect_uassoc() : usage variation ***\n";

//Initialize variables
$array1 = array("a" => "green", "c" => "blue", "red");
$array2 = array("a" => "green", "yellow", "red");
// define some class with method
class MyClass
{
	static function static_compare_func($a, $b) {
		return strcasecmp($a, $b);
	}

	public function class_compare_func($a, $b) {
		return strcasecmp($a, $b);
	}

}

echo "\n-- Testing array_intersect_uassoc() function using class with static method as callback --\n";
var_dump( array_intersect_uassoc($array1, $array2, array('MyClass','static_compare_func')) );
var_dump( array_intersect_uassoc($array1, $array2, 'MyClass::static_compare_func'));

echo "\n-- Testing array_intersect_uassoc() function using class with regular method as callback --\n";
$obj = new MyClass();
var_dump( array_intersect_uassoc($array1, $array2, array($obj,'class_compare_func')) );
?>
===DONE===
--EXPECTF--
*** Testing array_intersect_uassoc() : usage variation ***

-- Testing array_intersect_uassoc() function using class with static method as callback --
array(1) {
  ["a"]=>
  string(5) "green"
}
array(1) {
  ["a"]=>
  string(5) "green"
}

-- Testing array_intersect_uassoc() function using class with regular method as callback --
array(1) {
  ["a"]=>
  string(5) "green"
}
===DONE===
