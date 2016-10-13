--TEST--
get_class() tests
--FILE--
<?php

class foo {
	function bar () {
		var_dump(get_class());
	}
}

class foo2 extends foo {
}

foo::bar();
foo2::bar();

$f1 = new foo;
$f2 = new foo2;

$f1->bar();
$f2->bar();

var_dump(get_class());
var_dump(get_class("qwerty"));

var_dump(get_class($f1));
var_dump(get_class($f2));

echo "Done\n";
?>
--EXPECTF--	
Deprecated: Non-static method foo::bar() should not be called statically in %s on line %d
string(3) "foo"

Deprecated: Non-static method foo::bar() should not be called statically in %s on line %d
string(3) "foo"
string(3) "foo"
string(3) "foo"

Warning: get_class() called without object from outside a class in %s on line %d
bool(false)

Warning: get_class() expects parameter 1 to be object, string given in %s on line %d
bool(false)
string(3) "foo"
string(4) "foo2"
Done
