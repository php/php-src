--TEST--
Check for libsodium argon2i
--EXTENSIONS--
sodium
--SKIPIF--
<?php
if (!defined('SODIUM_CRYPTO_PWHASH_SALTBYTES')) print "skip libsodium without argon2i"; ?>
--FILE--
<?php
$passwd = 'password';

$hash = sodium_crypto_pwhash_str
  ($passwd, SODIUM_CRYPTO_PWHASH_OPSLIMIT_INTERACTIVE,
            SODIUM_CRYPTO_PWHASH_MEMLIMIT_INTERACTIVE);
var_dump(substr($hash, 0, strlen(SODIUM_CRYPTO_PWHASH_STRPREFIX)) ===
         SODIUM_CRYPTO_PWHASH_STRPREFIX);

$testHash = '$argon2i$v=19$m=4096,t=3,p=1$MzE4ODFiZWFlMjAzOWUAAA$FWUV6tsyJ32qThiLi1cCsLIbf3dIOG/RwXcTzt536KY';
$c = sodium_crypto_pwhash_str_verify($testHash, $passwd);
var_dump($c);

$testHash = '$argon2i$v=19$m=4096,t=0,p=1$c29tZXNhbHQAAAAAAAAAAA$JTBozgKQiCn5yKAm3Hz0vUSX/XgfqhZloNCxDWmeDr0';
$c = sodium_crypto_pwhash_str_verify($testHash, $passwd);
var_dump($c);

$c = sodium_crypto_pwhash_str_verify($hash, $passwd);
var_dump($c);

$c = sodium_crypto_pwhash_str_verify($hash, 'passwd');
var_dump($c);

$salt = random_bytes(SODIUM_CRYPTO_PWHASH_SALTBYTES);
$out_len = 100;
$key = sodium_crypto_pwhash
  ($out_len, $passwd, $salt,
   SODIUM_CRYPTO_PWHASH_OPSLIMIT_INTERACTIVE,
   SODIUM_CRYPTO_PWHASH_MEMLIMIT_INTERACTIVE,
   SODIUM_CRYPTO_PWHASH_ALG_DEFAULT);
var_dump(strlen($key) === $out_len);
$key2 = sodium_crypto_pwhash
  ($out_len, $passwd, $salt,
   SODIUM_CRYPTO_PWHASH_OPSLIMIT_INTERACTIVE,
   SODIUM_CRYPTO_PWHASH_MEMLIMIT_INTERACTIVE);
var_dump($key2 === $key);
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(true)
