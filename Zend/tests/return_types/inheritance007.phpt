--TEST--
Inheritance Hinting Compile Checking Failure Internal Classes
--INI--
opcache.enable_cli=1
--FILE--
<?php
class Foo {
    public static function test() : Traversable {
        return new ArrayIterator([1, 2]);
    }
}

class Bar extends Foo {
    public static function test() : ArrayObject {
        return new ArrayObject([1, 2]);
    }
}

echo get_class(Bar::test());

?>
--EXPECT--
ArrayObject
