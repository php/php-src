--TEST--
Closure 052: static::class in non-static closure in static method.
--FILE--
<?php

class A {
    static function foo() {
        $f = function() {
            return static::class;
        };
        return $f();
    }
}

class B extends A {}

var_dump(B::foo());
?>
--EXPECT--
string(1) "B"
