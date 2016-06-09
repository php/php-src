--TEST--
Error message should be the same independently of called function
--FILE--
<?php
class Foo {
	public int $bar;
}

function foo(&$x) {
}

$f = function (&$x) {};

$foo = new Foo();
try {
	foo($foo->bar);
} catch (Throwable $e) {
	echo $e->getMessage() . "\n";
}
try {
	$f($foo->bar);
} catch (Throwable $e) {
	echo $e->getMessage() . "\n";
}
--EXPECT--
Typed property Foo::$bar must not be referenced
Typed property Foo::$bar must not be referenced
