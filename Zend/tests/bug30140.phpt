--TEST--
Bug #30140 (Problem with array in static properties)
--FILE--
<?php
class A {
	public static $test1 = true;
	public static $test2 = array();
	public static $test3 = "str";
}

class B extends A {
}

A::$test1 = "x";
A::$test2 = "y";
A::$test3 = "z";
var_dump(A::$test1);
var_dump(A::$test2);
var_dump(A::$test3);
var_dump(B::$test1);
var_dump(B::$test2);
var_dump(B::$test3);
?>
--EXPECT--
string(1) "x"
string(1) "y"
string(1) "z"
string(1) "x"
string(1) "y"
string(1) "z"
