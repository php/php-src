--TEST--
Test BCRYPT fallback for crypt()
--FILE--
<?php
var_dump(crypt("test"));
var_dump(crypt("hello world"));
?>
--EXPECTF--
string(60) "$2y$10$%r(.{53})%r"
string(60) "$2y$10$%r(.{53})%r"
