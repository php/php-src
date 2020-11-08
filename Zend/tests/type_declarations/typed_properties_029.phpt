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
Fatal error: Uncaught TypeError: Cannot assign string to property Foo::$bar of type int in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
