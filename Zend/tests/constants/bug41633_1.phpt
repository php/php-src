--TEST--
Bug #41633.1 (self:: doesn't work for constants)
--FILE--
<?php
class Foo {
    const A = self::B;
    const B = "ok";
}
echo Foo::A."\n";
?>
--EXPECT--
ok
