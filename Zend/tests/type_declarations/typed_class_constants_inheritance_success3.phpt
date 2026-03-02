--TEST--
Typed class constants (inheritance; private constants)
--FILE--
<?php
class A {
    private const int CONST1 = 1;
}

class B extends A {
    public const string CONST1 = 'a';
}

var_dump(B::CONST1);
?>
--EXPECT--
string(1) "a"
