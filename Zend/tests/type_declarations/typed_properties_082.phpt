--TEST--
Test typed references to static properties 
--FILE--
<?php

class Test {
	public static int $x = 0;
}

class Test2 extends Test {
	public static $y = 1;
}

$x =& Test::$x;
try {
	$x = "foo";
} catch (TypeError $e) { var_dump($e->getMessage()); }
var_dump($x, Test::$x);

Test::$x =& Test2::$y; // remove the typed ref from $x
$x = "foo";
var_dump($x, Test::$x);

?>
--EXPECT--
string(45) "Cannot assign string to reference of type int"
int(0)
int(0)
string(3) "foo"
int(1)
