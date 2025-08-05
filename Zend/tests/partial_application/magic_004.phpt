--TEST--
PFA magic trampoline release used
--FILE--
<?php
class Foo {
    static function __callStatic($name, $args) {}
}
$a = Foo::method(?);

echo "OK";
?>
--EXPECT--
OK
