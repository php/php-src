--TEST--
Test crypt compatibility with password_hash
--FILE--
<?php
$secret = 'mysecret';

/* generate with password_hash, check with both */
$h = password_hash($secret, PASSWORD_BCRYPT);
var_dump($h, password_verify($secret, $h), $h===crypt($secret, $h));

/* generate with crypt, check with both */
$h = crypt($secret, crypt_gensalt(CRYPT_PREFIX_BLOWFISH));
var_dump($h, password_verify($secret, $h), $h===crypt($secret, $h));
?>
--EXPECTF--
string(60) "$2y$%s$%s"
bool(true)
bool(true)
string(60) "$2y$%s$%s"
bool(true)
bool(true)

