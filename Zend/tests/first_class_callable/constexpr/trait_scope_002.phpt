--TEST--
FCC for parent:: static method in trait default argument is resolved per using class
--FILE--
<?php

class P1 { public static function who() { return 'P1'; } }
class P2 { public static function who() { return 'P2'; } }
trait T {
    public static function make($f = parent::who(...)) { return $f(); }
}
class A extends P1 { use T; }
class B extends P2 { use T; }

echo A::make(), PHP_EOL;
echo B::make(), PHP_EOL;

?>
--EXPECT--
P1
P2
