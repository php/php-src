--TEST--
Test normal operation of password_hash() with Argon2i and Argon2id
--SKIPIF--
<?php
if (!defined('PASSWORD_ARGON2_PROVIDER')) die('skip password_hash not built with Argon2');
?>
--FILE--
<?php

$password = "the password for testing 12345!";

// To allow not repeating the same costly test
var_dump(PASSWORD_ARGON2I === 'argon2i');
var_dump(PASSWORD_ARGON2ID === 'argon2id');

$algos = [
    PASSWORD_ARGON2I,
    2,
    PASSWORD_ARGON2ID,
    3,
];
foreach ($algos as $algo) {
  echo ($hash = password_hash($password, $algo)), "\n";
  var_dump(password_verify($password, $hash));
}

echo "OK!";
?>
--EXPECTF--
bool(true)
bool(true)
$argon2i$v=19$m=%d,t=%d,p=%d$%s$%s
bool(true)
$argon2i$v=19$m=%d,t=%d,p=%d$%s$%s
bool(true)
$argon2id$v=19$m=%d,t=%d,p=%d$%s$%s
bool(true)
$argon2id$v=19$m=%d,t=%d,p=%d$%s$%s
bool(true)
OK!
