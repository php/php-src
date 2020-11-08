--TEST--
__get first parameter should be a string when typed
--FILE--
<?php
class Foo {
    function __get(int $name) {}
}
?>
--EXPECTF--
Fatal error: Foo::__get(): Parameter #1 ($name) must be of type string when declared in %s on line %d
