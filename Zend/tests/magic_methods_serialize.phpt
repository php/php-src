--TEST--
__serialize declaration
--FILE--
<?php
class Foo {
    static function __serialize($arguments) {}
}
?>
--EXPECTF--
Fatal error: Method Foo::__serialize() cannot take arguments in %s on line %d
