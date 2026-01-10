--TEST--
Typed class constants (inheritance success)
--FILE--
<?php
class A {
    public const CONST1 = 1;
    public const CONST2 = 1;
    public const mixed CONST3 = 1;
    public const iterable CONST4 = [];
}

class B extends A {
    public const int CONST1 = 0;
    public const mixed CONST2 = 0;
    public const mixed CONST3 = 0;
    public const array CONST4 = [];
}

var_dump(B::CONST1);
var_dump(B::CONST2);
var_dump(B::CONST3);
var_dump(B::CONST4);
?>
--EXPECT--
int(0)
int(0)
int(0)
array(0) {
}
