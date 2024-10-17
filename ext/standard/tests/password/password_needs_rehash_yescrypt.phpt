--TEST--
Test normal operation of password_needs_rehash() with Yescrypt
--FILE--
<?php

$hash = password_hash('test', PASSWORD_YESCRYPT);
var_dump(password_needs_rehash($hash, PASSWORD_YESCRYPT));
var_dump(password_needs_rehash($hash, PASSWORD_YESCRYPT, ['block_size' => PASSWORD_YESCRYPT_DEFAULT_BLOCK_SIZE * 2]));
var_dump(password_needs_rehash($hash, PASSWORD_YESCRYPT, ['block_count' => PASSWORD_YESCRYPT_DEFAULT_BLOCK_COUNT * 2]));
var_dump(password_needs_rehash($hash, PASSWORD_YESCRYPT, ['parallelism' => PASSWORD_YESCRYPT_DEFAULT_PARALLELISM + 1]));
var_dump(password_needs_rehash($hash, PASSWORD_YESCRYPT, ['time' => PASSWORD_YESCRYPT_DEFAULT_TIME + 1]));

echo "OK!";
?>
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
OK!
