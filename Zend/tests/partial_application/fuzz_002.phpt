--TEST--
Closure application fuzz 002
--FILE--
<?php
class Foo {
    static function __callStatic($name, $args) {}
}
Foo::method(?);

echo "OK";
?>
--EXPECTF--
OK
