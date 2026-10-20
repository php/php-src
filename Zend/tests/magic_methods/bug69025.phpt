--TEST--
Bug #69025 (Invalid read of size 4 when calling __callStatic)
--FILE--
<?php
class A {
    public static function __callStatic($method, $args)
    {
    }
}

A::init();
?>
OK
--EXPECT--
OK
