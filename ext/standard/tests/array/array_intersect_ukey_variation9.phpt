--TEST--
Test array_intersect_ukey() function : usage variation - Passing class/object methods to callback
--FILE--
<?php
/* Prototype  : array array_intersect_ukey(array arr1, array arr2 [, array ...], callback key_compare_func)
 * Description: Computes the intersection of arrays using a callback function on the keys for comparison.
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_intersect_ukey() : usage variation ***\n";

//Initialise arguments
$array1 = array('blue'  => 1, 'red'  => 2, 'green'  => 3, 'purple' => 4);
$array2 = array('green' => 5, 'blue' => 6, 'yellow' => 7, 'cyan'   => 8);

class MyClass
{
	static function static_compare_func($key1, $key2) {
		return strcasecmp($key1, $key2);
	}

	public function class_compare_func($key1, $key2) {
		return strcasecmp($key1, $key2);
	}

}

echo "\n-- Testing array_intersect_ukey() function using class with static method as callback --\n";
var_dump( array_intersect_ukey($array1, $array2, array('MyClass','static_compare_func')) );
var_dump( array_intersect_ukey($array1, $array2, 'MyClass::static_compare_func') );

echo "\n-- Testing array_intersect_uassoc() function using class with regular method as callback --\n";
$obj = new MyClass();
var_dump( array_intersect_ukey($array1, $array2, array($obj,'class_compare_func')) );
?>
===DONE===
--EXPECTF--
*** Testing array_intersect_ukey() : usage variation ***

-- Testing array_intersect_ukey() function using class with static method as callback --
array(2) {
  ["blue"]=>
  int(1)
  ["green"]=>
  int(3)
}
array(2) {
  ["blue"]=>
  int(1)
  ["green"]=>
  int(3)
}

-- Testing array_intersect_uassoc() function using class with regular method as callback --
array(2) {
  ["blue"]=>
  int(1)
  ["green"]=>
  int(3)
}
===DONE===
