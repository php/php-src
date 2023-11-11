--TEST--
Non-final Constants in traits can be overridden in derived classes
--FILE--
<?php

trait Foo {
    public const A = 123;
}

class Base {
    use Foo;
}

class Derived extends Base {
    public const A = 456;
}

echo Derived::A, PHP_EOL;
?>
--EXPECTF--
456
