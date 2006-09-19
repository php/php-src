--TEST--
array_walk() and objects
--FILE--
<?php

function walk($key, $value) { 
	var_dump($value, $key); 
}

class test {
	var $var_1 = "test_1";
	var $var_2 = "test_2";
}

$stdclass = new stdclass;
$stdclass->foo = "foo";
$stdclass->bar = "bar";
array_walk($stdclass, "walk");

$t = new test;
array_walk($t, "walk");

$var = array();
array_walk($var, "walk");
$var = "";
array_walk($var, "walk");

echo "Done\n";
?>
--EXPECTF--	
string(3) "foo"
string(3) "foo"
string(3) "bar"
string(3) "bar"
string(5) "var_1"
string(6) "test_1"
string(5) "var_2"
string(6) "test_2"

Warning: array_walk() [/phpmanual/function.array-walk.html]: The argument should be an array in %s on line %d
Done
