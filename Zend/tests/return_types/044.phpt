--TEST--
__set_state can only declare object as return
--FILE--
<?php
class Foo {
    public static function __set_state($properties): bool {}
}
?>
--EXPECTF--
Fatal error: Foo::__set_state(): Return type must be object when declared in %s on line %d
