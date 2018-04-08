--TEST--
Test normal operation of password_needs_rehash() with argon2
--SKIPIF--
<?php
if (!defined('PASSWORD_ARGON2I')) die('skip password_needs_rehash not built with Argon2');
?>
--FILE--
<?php

$hash = password_hash('test', PASSWORD_ARGON2I);
var_dump(password_needs_rehash($hash, PASSWORD_ARGON2I));
var_dump(password_needs_rehash($hash, PASSWORD_ARGON2I, ['memory_cost' => 1<<17]));
var_dump(password_needs_rehash($hash, PASSWORD_ARGON2I, ['time_cost' => 4]));
var_dump(password_needs_rehash($hash, PASSWORD_ARGON2I, ['threads' => 4]));
echo "OK!";
?>
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(true)
OK!
