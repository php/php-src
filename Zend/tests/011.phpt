--TEST--
property_exists() tests
--FILE--
<?php

class foo {
	public $pp1 = 1;
	private $pp2 = 2;
	protected $pp3 = 3;

	function bar() {
		var_dump(property_exists("foo","pp1"));
		var_dump(property_exists("foo","pp2"));
		var_dump(property_exists("foo","pp3"));
	}
}

class bar extends foo {
	function test() {
		var_dump(property_exists("foo","pp1"));
		var_dump(property_exists("foo","pp2"));
		var_dump(property_exists("foo","pp3"));
	}
}

var_dump(property_exists());
var_dump(property_exists(""));
var_dump(property_exists("foo","pp1"));
var_dump(property_exists("foo","pp2"));
var_dump(property_exists("foo","pp3"));
var_dump(property_exists("foo","nonexistent"));
var_dump(property_exists("fo","nonexistent"));
var_dump(property_exists("foo",""));
var_dump(property_exists("","test"));
var_dump(property_exists("",""));

$foo = new foo;

var_dump(property_exists($foo,"pp1"));
var_dump(property_exists($foo,"pp2"));
var_dump(property_exists($foo,"pp3"));
var_dump(property_exists($foo,"nonexistent"));
var_dump(property_exists($foo,""));
var_dump(property_exists(array(),"test"));
var_dump(property_exists(1,"test"));
var_dump(property_exists(true,"test"));

$foo->bar();

$bar = new bar;
$bar->test();

echo "Done\n";
?>
--EXPECTF--
Warning: property_exists() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: property_exists() expects exactly 2 parameters, 1 given in %s on line %d
NULL
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)

Warning: First parameter must either be an object or the name of an existing class in %s on line %d
NULL

Warning: First parameter must either be an object or the name of an existing class in %s on line %d
NULL

Warning: First parameter must either be an object or the name of an existing class in %s on line %d
NULL
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Done
