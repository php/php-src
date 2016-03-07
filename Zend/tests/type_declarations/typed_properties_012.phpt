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
Fatal error: Uncaught TypeError: foreach by reference over Foo is disallowed in %s:7
Stack trace:
#0 {main}
  thrown in %s on line 7


