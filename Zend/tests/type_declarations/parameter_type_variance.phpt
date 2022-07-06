--TEST--
Parameter variance with no type (class)
--FILE--
<?php

class Foo {
    function testParentClass(Foo $foo) {}
    function testBothClass(Foo $foo) {}
    function testChildClass($foo) {}
    function testNoneClass($foo) {}
}

class Bar extends Foo {
    function testParentClass($foo) {}
    function testBothClass(Foo $foo) {}
    function testChildClass(Foo $foo) {}
    function testNoneClass($foo) {}
}

?>
--EXPECTF--
Fatal error: Declaration of Bar::testChildClass(Foo $foo) must be compatible with Foo::testChildClass($foo) in %s on line %d
