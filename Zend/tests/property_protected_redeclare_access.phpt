--TEST--
Protected property access with redeclared properties in child classes
--FILE--
<?php

class B1 {
    protected $c = 1;
}

class C1_1 extends B1 {
    function f(B1 $x) {
        return $x->c;
    }
}

class C2_1 extends B1 {
    protected $c = 2;
}

echo "Instance properties: ";
var_dump((new C1_1)->f(new C2_1));

class B2 {
    protected static $c = 1;
}

class C1_2 extends B2 {
    function f(B2 $x) {
        return $x::$c;
    }
}

class C2_2 extends B2 {
    protected static $c = 2;
}

echo "Static properties: ";
var_dump((new C1_2)->f(new C2_2));

class B3 {
    protected const c = 1;
}

class C1_3 extends B3 {
    function f(B3 $x) {
        return $x::c;
    }
}

class C2_3 extends B3 {
    protected const c = 2;
}

echo "Constants: ";
var_dump((new C1_3)->f(new C2_3));

?>
--EXPECT--
Instance properties: int(2)
Static properties: int(2)
Constants: int(2)
