--TEST--
Test typed properties respect strict types (on)
--FILE--
<?php
declare(strict_types=1);

class Foo {
	public int $bar;
}

$foo = new Foo;
$foo->bar = "1";
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Typed property Foo::$bar must be int, string used in %s:9
Stack trace:
#0 {main}
  thrown in %s on line 9
