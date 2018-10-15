--TEST--
Test normal operation of password_needs_rehash() with Argon2i and Argon2id
--SKIPIF--
<?php
if (!defined('PASSWORD_ARGON2I')) die('skip password_needs_rehash not built with Argon2');
if (!defined('PASSWORD_ARGON2ID')) die('skip password_hash not built with Argon2');
?>
--FILE--
<?php

$hash = password_hash('test', PASSWORD_ARGON2I);
var_dump(password_needs_rehash($hash, PASSWORD_ARGON2I));
var_dump(password_needs_rehash($hash, PASSWORD_ARGON2I, ['memory_cost' => 1<<17]));
var_dump(password_needs_rehash($hash, PASSWORD_ARGON2I, ['time_cost' => 4]));
var_dump(password_needs_rehash($hash, PASSWORD_ARGON2I, ['threads' => 4]));

$hash = password_hash('test', PASSWORD_ARGON2ID);
var_dump(password_needs_rehash($hash, PASSWORD_ARGON2ID));
var_dump(password_needs_rehash($hash, PASSWORD_ARGON2ID, ['memory_cost' => 1<<17]));
var_dump(password_needs_rehash($hash, PASSWORD_ARGON2ID, ['time_cost' => 4]));
var_dump(password_needs_rehash($hash, PASSWORD_ARGON2ID, ['threads' => 4]));
echo "OK!";
?>
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
OK!
