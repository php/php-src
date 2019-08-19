--TEST--
get_class() tests
--FILE--
<?php

class foo {
	function bar () {
		var_dump(get_class());
	}
    function testNull ()
    {
        var_dump(get_class(null));
    }
}

class foo2 extends foo {
}

$f1 = new foo;
$f2 = new foo2;

$f1->bar();
$f2->bar();

var_dump(get_class());
var_dump(get_class("qwerty"));

var_dump(get_class($f1));
var_dump(get_class($f2));

$f1->testNull();

echo "Done\n";
?>
--EXPECTF--
string(3) "foo"
string(3) "foo"

Warning: get_class() called without object from outside a class in %s on line %d
bool(false)

Warning: get_class() expects parameter 1 to be object, string given in %s on line %d
bool(false)
string(3) "foo"
string(4) "foo2"

Warning: get_class() expects parameter 1 to be object, null given in %s on line %d
bool(false)
Done
