--TEST--
FCC for self:: static method in trait constant and property default is resolved per using class
--FILE--
<?php

trait T {
    const C = self::who(...);
    public $p = self::who(...);
}
class A { use T; public static function who() { return 'A'; } }
class B { use T; public static function who() { return 'B'; } }

echo (A::C)(), PHP_EOL;
echo (B::C)(), PHP_EOL;
echo ((new A)->p)(), PHP_EOL;
echo ((new B)->p)(), PHP_EOL;

?>
--EXPECT--
A
B
A
B
