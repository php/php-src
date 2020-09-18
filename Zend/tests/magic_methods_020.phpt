--TEST--
__set_state first parameter must be an array
--FILE--
<?php

class Foo {
    public static function __set_state(int $properties) {}
}

?>
--EXPECTF--
Fatal error: Foo::__set_state(): Parameter #1 ($properties) must be of type array when declared in %s on line %d
