--TEST--
Test error operation of password_verify()
--FILE--
<?php
//-=-=-=-

var_dump(password_verify());

var_dump(password_verify("foo"));

?>
--EXPECTF--
Warning: password_verify() expects exactly 2 parameters, 0 given in %s on line %d
bool(false)

Warning: password_verify() expects exactly 2 parameters, 1 given in %s on line %d
bool(false)

