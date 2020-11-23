--TEST--
Parameter variance with no type (builtin)
--FILE--
<?php

class Foo {
    function testParentBuiltin(int $foo) {}
    function testBothBuiltin(int $foo) {}
    function testChildBuiltin($foo) {}
    function testNoneBuiltin($foo) {}
}

class Bar extends Foo {
    function testParentBuiltin($foo) {}
    function testBothBuiltin(int $foo) {}
    function testChildBuiltin(int $foo) {}
    function testNoneBuiltin($foo) {}
}

?>
--EXPECTF--
Fatal error: Declaration of Bar::testChildBuiltin(int $foo) must be compatible with Foo::testChildBuiltin($foo) in %s on line %d
