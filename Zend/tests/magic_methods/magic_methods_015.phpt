--TEST--
__call first parameter should be a string typed
--FILE--
<?php
class Foo {
    function __call(int $name, array $arguments) {}
}
?>
--EXPECTF--
Fatal error: Foo::__call(): Parameter #1 ($name) must be of type string when declared in %s on line %d
