--TEST--
Compile-time return type covariance; extends class by name

--FILE--
<?php

class A {
    function foo(): A {}
}

class B extends A {
    function foo(): B {}
}

--EXPECT--
