--TEST--
Property fetch in enum initializers
--FILE--
<?php

enum A: string {
    case B = 'C';
}

enum D: string {
    case E = A::B->name;
    case F = A::B->value;
}

enum G: string {
    case H = A::B?->name;
    case I = A::B?->value;
}

var_dump(D::E->value);
var_dump(D::F->value);
var_dump(G::H->value);
var_dump(G::I->value);

?>
--EXPECT--
string(1) "B"
string(1) "C"
string(1) "B"
string(1) "C"
