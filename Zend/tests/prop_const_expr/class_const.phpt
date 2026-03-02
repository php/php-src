--TEST--
Allow fetching properties in class constants
--FILE--
<?php

enum A: string {
    case Case = 'A::Case';
}

class C {
    const A_name = A::Case->name;
    const A_value = A::Case->value;
    const A_name_nullsafe = A::Case?->name;
    const A_value_nullsafe = A::Case?->value;
}

var_dump(C::A_name);
var_dump(C::A_value);
var_dump(C::A_name_nullsafe);
var_dump(C::A_value_nullsafe);

?>
--EXPECT--
string(4) "Case"
string(7) "A::Case"
string(4) "Case"
string(7) "A::Case"
