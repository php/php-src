--TEST--
Test error operation of password_hash()
--FILE--
<?php
//-=-=-=-

var_dump(password_hash());

var_dump(password_hash("foo", array()));

var_dump(password_hash("foo", "bar", new StdClass));

var_dump(password_hash("foo", "bar", "baz"));

var_dump(password_hash(array(), PASSWORD_BCRYPT));

var_dump(password_hash("123", PASSWORD_BCRYPT, array("salt" => array())));

?>
--EXPECTF--
Warning: password_hash() expects at least 1 parameter, 0 given in %s on line %d
NULL

Warning: password_hash() expects parameter 2 to be string, array given in %s on line %d
NULL

Warning: password_hash(): Unknown password hashing algorithm: bar in %s on line %d
NULL

Warning: password_hash() expects parameter 3 to be array, string given in %s on line %d
NULL

Warning: password_hash() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: password_hash(): Non-string salt parameter supplied in %s on line %d
NULL

