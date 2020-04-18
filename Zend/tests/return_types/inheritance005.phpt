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

echo get_class(Bar::test());

?>
--EXPECT--
Bar
