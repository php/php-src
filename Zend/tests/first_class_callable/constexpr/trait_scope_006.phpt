--TEST--
FCC for named class public static method in trait default argument is shared across using classes
--FILE--
<?php

trait T {
    public static function make($f = A::who(...)) { return $f(); }
}
class A { use T; public static function who() { return 'A'; } }
class B { use T; }

echo A::make(), PHP_EOL;
echo B::make(), PHP_EOL;
echo A::make(), PHP_EOL;

?>
--EXPECT--
A
A
A
