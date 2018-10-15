--TEST--
Test error operation of password_hash() with argon2
--SKIPIF--
<?php
if (!defined('PASSWORD_ARGON2I')) die('skip password_hash not built with Argon2');
?>
--FILE--
<?php
var_dump(password_hash('test', PASSWORD_ARGON2I, ['memory_cost' => 0]));
var_dump(password_hash('test', PASSWORD_ARGON2I, ['time_cost' => 0]));
var_dump(password_hash('test', PASSWORD_ARGON2I, ['threads' => 0]));
?>
--EXPECTF--
Warning: password_hash(): Memory cost is outside of allowed memory range in %s on line %d
NULL

Warning: password_hash(): Time cost is outside of allowed time range in %s on line %d
NULL

Warning: password_hash(): Invalid number of threads in %s on line %d
NULL
