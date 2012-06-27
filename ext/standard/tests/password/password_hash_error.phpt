--TEST--
Test error operation of password_hash()
--FILE--
<?php
//-=-=-=-

var_dump(password_hash());

var_dump(password_hash("foo", array()));

var_dump(password_hash("foo", "bar", new StdClass));

var_dump(password_hash("foo", "bar", "baz"));

var_dump(password_hash(123));

var_dump(password_hash("123", PASSWORD_BCRYPT, array("salt" => 13)));

?>
--EXPECTF--
Warning: password_hash() expects at least 1 parameter, 0 given in %s on line %d
bool(false)

Warning: password_hash() expects parameter 2 to be string, array given in %s on line %d
bool(false)

Warning: password_hash(): Unknown password hashing algorithm: bar in %s on line %d
bool(false)

Warning: password_hash() expects parameter 3 to be array, string given in %s on line %d
bool(false)

Warning: password_hash(): Password must be a string in %s on line %d
bool(false)

Warning: password_hash(): Non-string salt parameter supplied in %s on line %d
bool(false)

