--TEST--
Test typed disallow fetch reference for foreach
--FILE--
<?php
class Foo {
	public int $bar = 1;
}

$foo = new Foo();
foreach ($foo as &$prop);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Typed properties exist in Foo: foreach by reference is disallowed in %s:7
Stack trace:
#0 {main}
  thrown in %s on line 7


