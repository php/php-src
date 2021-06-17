--TEST--
Partial application magic trampoline release
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
