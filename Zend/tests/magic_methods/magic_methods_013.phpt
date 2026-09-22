--TEST--
__isset first parameter should be a string when typed
--FILE--
<?php
class Foo {
    function __isset(\stdClass $name) {}
}
?>
--EXPECTF--
Fatal error: Foo::__isset(): Parameter #1 ($name) must be of type string when declared in %s on line %d
