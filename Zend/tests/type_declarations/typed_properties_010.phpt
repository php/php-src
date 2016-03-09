--TEST--
Test typed properties disallow fetch reference for func arg
--FILE--
<?php
class Foo {
	public int $bar = 1;
}

$cb = function(int &$bar) {
	var_dump($bar);
};

$foo = new Foo();
$cb($foo->bar);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Typed property Foo::$bar must not be referenced in %s:11
Stack trace:
#0 {main}
  thrown in %s on line 11


