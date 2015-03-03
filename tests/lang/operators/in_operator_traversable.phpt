--TEST--
In operator tests for traversables
--FILE--
<?php

function gen() {
	echo "1";
	yield "bar";
	echo "2";
	yield "foo";
	echo "3";
}

var_dump("foo" in gen());
var_dump("php" in gen());

?>
--EXPECT--
12bool(true)
123bool(false)
