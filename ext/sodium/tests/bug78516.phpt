--TEST--
Bug #78516 (password_hash(): Memory cost is not in allowed range)
--SKIPIF--
<?php
if (!extension_loaded('sodium')) die('skip sodium extension not available');
if (!defined('PASSWORD_ARGON2ID')) die('skip PASSWORD_ARGON2ID not available');
?>
--FILE--
<?php
$pass = password_hash('secret', PASSWORD_ARGON2ID, ['memory_cost' => 8191]);
password_needs_rehash($pass, PASSWORD_ARGON2ID, ['memory_cost' => 8191]);
var_dump(password_get_info($pass)['options']['memory_cost']);
$pass = password_hash('secret', PASSWORD_ARGON2I, ['memory_cost' => 8191]);
password_needs_rehash($pass, PASSWORD_ARGON2I, ['memory_cost' => 8191]);
var_dump(password_get_info($pass)['options']['memory_cost']);
?>
--EXPECT--
int(8191)
int(8191)
