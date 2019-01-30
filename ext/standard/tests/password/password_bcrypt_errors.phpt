--TEST--
Test error operation of password_hash() with bcrypt hashing
--FILE--
<?php
//-=-=-=-

var_dump(password_hash("foo", PASSWORD_BCRYPT, array("cost" => 3)));

var_dump(password_hash("foo", PASSWORD_BCRYPT, array("cost" => 32)));

?>
--EXPECTF--
Warning: password_hash(): Invalid bcrypt cost parameter specified: 3 in %s on line %d
NULL

Warning: password_hash(): Invalid bcrypt cost parameter specified: 32 in %s on line %d
NULL
