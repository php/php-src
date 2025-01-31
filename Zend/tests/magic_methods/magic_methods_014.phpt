--TEST--
__unset first parameter should be a string when typed
--FILE--
<?php
class Foo {
    function __unset(array $name) {}
}
?>
--EXPECTF--
Fatal error: Foo::__unset(): Parameter #1 ($name) must be of type string when declared in %s on line %d
