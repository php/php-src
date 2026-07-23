--TEST--
PFA magic trampoline release (result used)
--FILE--
<?php
class Foo {
    static function __callStatic($name, $args) {}
}
$a = Foo::method(?);
(void)Foo::method(?);

echo "OK";
?>
--EXPECT--
OK
