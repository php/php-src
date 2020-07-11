--TEST--
Closure 051: static::class in static closure in static method.
--FILE--
<?php

class A {
    static function foo() {
        $f = static function() {
            return static::class;
        };
        return $f();
    }
}

class B extends A {}

var_dump(B::foo());
--EXPECT--
string(1) "B"
