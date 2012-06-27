--TEST--
Test error operation of password_make_salt()
--FILE--
<?php
//-=-=-=-

var_dump(password_make_salt());

var_dump(password_make_salt("foo"));

var_dump(password_make_salt(-1));

?>
--EXPECTF--
Warning: password_make_salt() expects at least 1 parameter, 0 given in %s on line %d
bool(false)

Warning: password_make_salt() expects parameter 1 to be long, string given in %s on line %d
bool(false)

Warning: password_make_salt(): Length cannot be less than or equal zero: -1 in %s on line %d
bool(false)

