--TEST--
GH-7821: Can't use arbitrary constant expressions in enum cases
--FILE--
<?php

interface I {
    public const A = 'A';
    public const B = 'B';
}

enum B: string implements I {
    case C = I::A;
    case D = self::B;
}

var_dump(B::A);
var_dump(B::B);
var_dump(B::C->value);
var_dump(B::D->value);

?>
--EXPECT--
string(1) "A"
string(1) "B"
string(1) "A"
string(1) "B"
