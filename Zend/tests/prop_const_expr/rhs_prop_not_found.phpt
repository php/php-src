--TEST--
Property not found error
--FILE--
<?php

enum A {
    case B;
}

const A_prop = A::B->prop;
var_dump(A_prop);

const A_prop_nullsafe = A::B?->prop;
var_dump(A_prop_nullsafe);

?>
--EXPECTF--
Warning: Undefined property: A::$prop in %s on line %d
NULL

Warning: Undefined property: A::$prop in %s on line %d
NULL
