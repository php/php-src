--TEST--
Bug #75221 (Argon2i always throws NUL at the end)
--SKIPIF--
<?php
if (!defined('PASSWORD_ARGON2I')) die('skip password_hash not built with Argon2');
?>
--FILE--
<?php
$hash = password_hash(
    "php",
    PASSWORD_ARGON2I,
    ['memory_cost' => 64 << 10, 'time_cost' => 4, 'threads' => 1]
);
var_dump(substr($hash, -1, 1) !== "\0");
?>
--EXPECT--
bool(true)
