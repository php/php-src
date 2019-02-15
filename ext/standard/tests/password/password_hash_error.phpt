--TEST--
Test error operation of password_hash()
--FILE--
<?php
//-=-=-=-

var_dump(password_hash());

var_dump(password_hash("foo"));

var_dump(password_hash("foo", array()));

var_dump(password_hash("foo", 19, new StdClass));

var_dump(password_hash("foo", PASSWORD_BCRYPT, "baz"));

var_dump(password_hash(array(), PASSWORD_BCRYPT));

?>
--EXPECTF--
Warning: password_hash() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: password_hash() expects at least 2 parameters, 1 given in %s on line %d
NULL

Notice: Array to string conversion in %s on line %d

Warning: password_hash(): Unknown password hashing algorithm: Array in %s on line %d
NULL

Warning: password_hash(): Unknown password hashing algorithm: 19 in %s on line %d
NULL

Warning: password_hash() expects parameter 3 to be array, string given in %s on line %d
NULL

Warning: password_hash() expects parameter 1 to be string, array given in %s on line %d
NULL
