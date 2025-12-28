--TEST--
__set first parameter should be a string when typed
--FILE--
<?php
class Foo {
    function __set(\Countable $name, $value) {}
}
?>
--EXPECTF--
Fatal error: Foo::__set(): Parameter #1 ($name) must be of type string when declared in %s on line %d
