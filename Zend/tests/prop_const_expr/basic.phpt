--TEST--
Allow fetching properties in constant expressions on enums
--FILE--
<?php

enum A: string {
    case Case = 'A::Case';
}

const A_name = A::Case->name;
const A_value = A::Case->value;

var_dump(A_name);
var_dump(A_value);

const A_name_nullsafe = A::Case?->name;
const A_value_nullsafe = A::Case?->value;

var_dump(A_name_nullsafe);
var_dump(A_value_nullsafe);

?>
--EXPECT--
string(4) "Case"
string(7) "A::Case"
string(4) "Case"
string(7) "A::Case"
