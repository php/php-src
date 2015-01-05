--TEST--
intdiv functionality
--FILE--
<?php
var_dump(intdiv(3, 2));
var_dump(intdiv(-3, 2));
var_dump(intdiv(3, -2));
var_dump(intdiv(-3, -2));
var_dump(intdiv(PHP_INT_MAX, PHP_INT_MAX));
var_dump(intdiv(PHP_INT_MIN, PHP_INT_MIN));
var_dump(intdiv(PHP_INT_MIN, -1));
// (int)(PHP_INT_MAX / 3) gives a different result
var_dump(intdiv(PHP_INT_MAX, 3));

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

Warning: intdiv(): Division by zero in %s on line 19
bool(false)
