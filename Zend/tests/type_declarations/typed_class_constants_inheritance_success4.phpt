--TEST--
Typed class constants (composition; traits)
--FILE--
<?php

const G = new stdClass();

enum E {
    case Case1;
}

trait T {
    public const int CONST1 = 1;
    public const ?array CONST2 = [];
    public const E CONST3 = E::Case1;
    public const stdClass CONST4 = G;
}

class C {
    use T;

    public const int CONST1 = 1;
    public const ?array CONST2 = [];
    public const E CONST3 = E::Case1;
    public const stdClass CONST4 = G;
}

var_dump(C::CONST1);
var_dump(C::CONST2);
var_dump(C::CONST3);
var_dump(C::CONST4);
?>
--EXPECT--
int(1)
array(0) {
}
enum(E::Case1)
object(stdClass)#1 (0) {
}
