--TEST--
Closure 050: non-static closure in non-static method.

--FILE--
<?php

class A {
    function a() {
        $f = function() {
            return static::class;
        };
        return [static::class, $f()];
    }
}

class B extends A {}

$b = new B;
list($method, $closure) = $b->a();
var_dump($method);
var_dump($closure);

--EXPECT--
string(1) "B"
string(1) "B"
