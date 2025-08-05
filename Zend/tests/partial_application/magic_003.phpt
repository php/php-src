--TEST--
PFA magic trampoline release unused
--FILE--
<?php
class Foo {
    static function __callStatic($name, $args) {}
}
Foo::method(?);

echo "OK";
?>
--EXPECT--
OK
