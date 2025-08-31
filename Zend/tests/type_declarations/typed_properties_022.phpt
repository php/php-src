--TEST--
Test typed properties delay type check on ast
--FILE--
<?php
class Foo {
    public int $bar = BAR::BAZ * 2;
}

$foo = new Foo();
?>
--EXPECTF--
Fatal error: Uncaught Error: Class "BAR" not found in %s:%d
Stack trace:
#0 %s(%d): [constant expression]()
#1 {main}
  thrown in %s on line %d
