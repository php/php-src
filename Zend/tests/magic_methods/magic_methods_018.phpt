--TEST--
__callStatic second parameter should be an array typed
--FILE--
<?php
class Foo {
    static function __callStatic(string $name, \Arguments $args) {}
}
?>
--EXPECTF--
Fatal error: Foo::__callStatic(): Parameter #2 ($args) must be of type array when declared in %s on line %d
