--TEST--
Test typed properties inheritance (scalar)
--FILE--
<?php
class Foo {
    public int $qux;
}

class Bar extends Foo {
    public string $qux;
}
?>
--EXPECTF--
Fatal error: Type of Bar::$qux must be int (as in class Foo) in %s on line 6
