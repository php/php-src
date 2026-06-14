--TEST--
Bug #41633.4 (self:: doesn't work for constants)
--FILE--
<?php
class Foo {
    const A = self::B;
    const B = "ok";
}
var_dump(defined("Foo::A"));
?>
--EXPECT--
bool(true)
