--TEST--
FCC for self:: static method in trait default argument is resolved per using class
--FILE--
<?php

trait T {
    public static function make($f = self::who(...)) { return $f(); }
}
class A { use T; public static function who() { return 'A'; } }
class B { use T; public static function who() { return 'B'; } }

echo A::make(), PHP_EOL;
echo B::make(), PHP_EOL;

?>
--EXPECT--
A
B
