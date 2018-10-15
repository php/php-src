--TEST--
Internal covariant return type of self
--FILE--
<?php
class Foo {
    public static function test() : self {
        return new Foo;
    }
}

class Bar extends Foo {
    public static function test() : self {
        return new Bar;
    }
}
--EXPECTF--
Fatal error: Declaration of Bar::test(): Bar must be compatible with Foo::test(): Foo in %sinheritance005.php on line 12
