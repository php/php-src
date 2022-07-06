--TEST--
__call second parameter should be an array when typed
--FILE--
<?php
class Foo {
    function __call(string $name, \Arguments $arguments) {}
}
?>
--EXPECTF--
Fatal error: Foo::__call(): Parameter #2 ($arguments) must be of type array when declared in %s on line %d
