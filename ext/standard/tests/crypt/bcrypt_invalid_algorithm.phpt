--TEST--
Test BCRYPT with invalid algorithm
--FILE--
<?php
var_dump(crypt("test", "$23$04$1234567890123456789012345"));
var_dump(crypt("test", "$20$04$1234567890123456789012345"));
var_dump(crypt("test", "$2g$04$1234567890123456789012345"));
?>
--EXPECTF--
string(2) "*0"
string(2) "*0"
string(2) "*0"
