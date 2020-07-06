--TEST--
Typed class constants (inheritance; private constants)
--FILE--
<?php
class A {
    private const int A = 1;
}

class B extends A {
    public const string A = 'a';
}

echo B::A;
?>
--EXPECT--
a
