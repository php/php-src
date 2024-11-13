--TEST--
__callStatic first parameter should be a string typed
--FILE--
<?php
class Foo {
    static function __callStatic(int $name, array $arguments) {}
}
?>
--EXPECTF--
Fatal error: Foo::__callStatic(): Parameter #1 ($name) must be of type string when declared in %s on line %d
