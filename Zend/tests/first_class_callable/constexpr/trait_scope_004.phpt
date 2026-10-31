--TEST--
FCC for self:: private static method in trait default argument does not leak to other using classes
--FILE--
<?php

trait T {
    public static function make($f = self::who(...)) { return $f(); }
}
class A { use T; private static function who() { return 'A'; } }
class B { use T; }

echo A::make(), PHP_EOL;
try {
    B::make();
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
A
Error: Call to undefined method B::who()
