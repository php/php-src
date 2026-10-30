--TEST--
FCC for named class private static method in trait default argument checks visibility per using class
--FILE--
<?php

trait T {
    public static function make($f = A::who(...)) { return $f(); }
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
Error: Call to private method A::who() from scope B
