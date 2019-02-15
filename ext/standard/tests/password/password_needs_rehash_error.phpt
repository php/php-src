--TEST--
Test error operation of password_needs_rehash()
--FILE--
<?php
//-=-=-=-
var_dump(password_needs_rehash());

var_dump(password_needs_rehash(''));

var_dump(password_needs_rehash('', []));

var_dump(password_needs_rehash(array(), PASSWORD_BCRYPT));

var_dump(password_needs_rehash("", PASSWORD_BCRYPT, "foo"));

echo "OK!";
?>
--EXPECTF--
Warning: password_needs_rehash() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: password_needs_rehash() expects at least 2 parameters, 1 given in %s on line %d
NULL
bool(false)

Warning: password_needs_rehash() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: password_needs_rehash() expects parameter 3 to be array, string given in %s on line %d
NULL
OK!
