--TEST--
intdiv functionality
--FILE--
<?php

const INT_MAX_64 = 0x7FFFFFFFFFFFFFFF;
const INT_MIN_64 = -INT_MAX_64 - 1;
var_dump(intdiv(3, 2));
var_dump(intdiv(-3, 2));
var_dump(intdiv(3, -2));
var_dump(intdiv(-3, -2));
var_dump(intdiv(INT_MAX_64, INT_MAX_64));
var_dump(intdiv(INT_MIN_64, INT_MIN_64));
var_dump(intdiv(INT_MIN_64, -1));
// (int)(INT_MAX_64 / 3) gives a different result
var_dump(intdiv(INT_MAX_64, 3));

// Thanks to zend_parse_parameters, these should also work
var_dump(intdiv(3.0, '2'));
var_dump(intdiv('-3', 2.0));
var_dump(intdiv(3.0, -2.0));
var_dump(intdiv('-3.0', '-2.0'));

// div0! yay!
var_dump(intdiv(1, 0));
?>

--EXPECTF--
int(1)
int(-1)
int(-1)
int(1)
int(1)
int(1)
int(%d)
int(3074457345618258602)
int(1)
int(-1)
int(-1)
int(1)

Warning: intdiv(): Division by zero in %s on line 22
bool(false)
