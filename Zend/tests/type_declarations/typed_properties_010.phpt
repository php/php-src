--TEST--
Test typed disallow fetch reference for func arg
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
Fatal error: Uncaught TypeError: fetching reference to Foo::$bar is disallowed in %s:11
Stack trace:
#0 {main}
  thrown in %s on line 11


