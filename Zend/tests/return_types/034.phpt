--TEST--
__isset can only declare a boolean return type
--FILE--
<?php
class Foo {
    function __isset($name) : \stdClass|bool {}
}
?>
--EXPECTF--
Fatal error: Foo::__isset(): Return type must be bool when declared in %s on line %d
