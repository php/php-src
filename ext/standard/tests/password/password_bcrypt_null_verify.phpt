--TEST--
password_verify() rejects bcrypt passwords containing null bytes
--FILE--
<?php
$hash = password_hash("foo", PASSWORD_BCRYPT);

var_dump(password_verify("foo", $hash));
var_dump(password_verify("foo\0bar", $hash));
var_dump(password_verify("\0foo", $hash));
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
