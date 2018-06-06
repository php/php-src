--TEST--
Parameter variance with no type
--FILE--
<?php

class Foo {
    function testParentClass(Foo $foo) {}
    function testBothClass(Foo $foo) {}
    function testChildClass($foo) {}
    function testNoneClass($foo) {}

    function testParentBuiltin(int $foo) {}
    function testBothBuiltin(int $foo) {}
    function testChildBuiltin($foo) {}
    function testNoneBuiltin($foo) {}
}

class Bar extends Foo {
    function testParentClass($foo) {}
    function testBothClass(Foo $foo) {}
    function testChildClass(Foo $foo) {}
    function testNoneClass($foo) {}

    function testParentBuiltin($foo) {}
    function testBothBuiltin(int $foo) {}
    function testChildBuiltin(int $foo) {}
    function testNoneBuiltin($foo) {}
}

?>
--EXPECTF--
Warning: Declaration of Bar::testChildClass(Foo $foo) should be compatible with Foo::testChildClass($foo) in %s on line %d

Warning: Declaration of Bar::testChildBuiltin(int $foo) should be compatible with Foo::testChildBuiltin($foo) in %s on line %d
