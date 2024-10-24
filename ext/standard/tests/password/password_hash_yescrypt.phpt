--TEST--
Test normal operation of password_hash() with Yescrypt
--FILE--
<?php

$password = "the password for testing 12345!";

$hash = password_hash($password, PASSWORD_YESCRYPT);
var_dump(password_verify($password, $hash));
var_dump(password_get_info($hash)['algo']);

echo "OK!";
?>
--EXPECT--
bool(true)
string(1) "y"
OK!
