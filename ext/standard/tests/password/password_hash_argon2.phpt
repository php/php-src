--TEST--
Test normal operation of password_hash() with Argon2i and Argon2id
--SKIPIF--
<?php
if (!defined('PASSWORD_ARGON2I')) die('skip password_hash not built with Argon2');
if (!defined('PASSWORD_ARGON2ID')) die('skip password_hash not built with Argon2');
--FILE--
<?php

$password = "the password for testing 12345!";

$algos = [
    PASSWORD_ARGON2I,
    'argon2i',
    2,
    PASSWORD_ARGON2ID,
    'argon2id',
    3,
];
foreach ($algos as $algo) {
  $hash = password_hash($password, $algo);
  var_dump(password_verify($password, $hash));
  var_dump(password_get_info($hash)['algo']);
}

echo "OK!";
?>
--EXPECT--
bool(true)
string(7) "argon2i"
bool(true)
string(7) "argon2i"
bool(true)
string(7) "argon2i"
bool(true)
string(8) "argon2id"
bool(true)
string(8) "argon2id"
bool(true)
string(8) "argon2id"
OK!
