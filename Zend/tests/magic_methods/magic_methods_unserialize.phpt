--TEST--
__unserialize declaration
--FILE--
<?php
class Foo {
    static function __unserialize($data, $value) {}
}
?>
--EXPECTF--
Fatal error: Method Foo::__unserialize() must take exactly 1 argument in %s on line %d
