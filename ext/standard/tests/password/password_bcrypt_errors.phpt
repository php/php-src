--TEST--
Test error operation of password_hash() with bcrypt hashing
--FILE--
<?php
//-=-=-=-

var_dump(password_hash("foo", PASSWORD_BCRYPT, array("cost" => 3)));

var_dump(password_hash("foo", PASSWORD_BCRYPT, array("cost" => 32)));

var_dump(password_hash("foo", PASSWORD_BCRYPT, array("salt" => "foo")));

var_dump(password_hash("foo", PASSWORD_BCRYPT, array("salt" => "123456789012345678901")));

var_dump(password_hash("foo", PASSWORD_BCRYPT, array("salt" => 123)));

var_dump(password_hash("foo", PASSWORD_BCRYPT, array("cost" => "foo")));

?>
--EXPECTF--
Warning: password_hash(): Invalid bcrypt cost parameter specified: 3 in %s on line %d
NULL

Warning: password_hash(): Invalid bcrypt cost parameter specified: 32 in %s on line %d
NULL

Deprecated: password_hash(): Use of the 'salt' option to password_hash is deprecated in %s on line %d

Warning: password_hash(): Provided salt is too short: 3 expecting 22 in %s on line %d
NULL

Deprecated: password_hash(): Use of the 'salt' option to password_hash is deprecated in %s on line %d

Warning: password_hash(): Provided salt is too short: 21 expecting 22 in %s on line %d
NULL

Deprecated: password_hash(): Use of the 'salt' option to password_hash is deprecated in %s on line %d

Warning: password_hash(): Provided salt is too short: 3 expecting 22 in %s on line %d
NULL

Warning: password_hash(): Invalid bcrypt cost parameter specified: 0 in %s on line %d
NULL
