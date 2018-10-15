--TEST--
Test normal operation of password_hash() with argon2
--SKIPIF--
<?php
if (!defined('PASSWORD_ARGON2I')) die('skip password_hash not built with Argon2');
--FILE--
<?php

$password = "the password for testing 12345!";

$hash = password_hash($password, PASSWORD_ARGON2I);
var_dump(password_verify($password, $hash));

echo "OK!";
?>
--EXPECT--
bool(true)
OK!
