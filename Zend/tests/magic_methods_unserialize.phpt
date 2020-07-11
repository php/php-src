--TEST--
__unserialize declaration
--FILE--
<?php
class Foo {
    static function __unserialize($data, $value) {}
}
?>
--EXPECTF--
Warning: The magic method Foo::__unserialize() cannot be static in %s on line %d

Fatal error: Method Foo::__unserialize() must take exactly 1 argument in %s on line %d
