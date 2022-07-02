--TEST--
Closure 056: self::class in non-static closure in static method.
--FILE--
<?php

class A {
    static function foo() {
        $f = function() {
            return self::class;
        };
        return $f();
    }
}

class B extends A {}

var_dump(B::foo());
?>
--EXPECT--
string(1) "A"
