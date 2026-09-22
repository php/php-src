--TEST--
sodium pwhash argument errors throw ValueError and keep the whole backtrace scrubbed
--EXTENSIONS--
sodium
--SKIPIF--
<?php
if (!defined('SODIUM_CRYPTO_PWHASH_SALTBYTES')) print "skip libsodium without argon2";
?>
--FILE--
<?php
function wrap(string $password): void
{
    sodium_crypto_pwhash_str($password, SODIUM_CRYPTO_PWHASH_OPSLIMIT_INTERACTIVE, 1);
}

$secret = "hunter2-secret";
wrap($secret);
?>
--EXPECTF--
Fatal error: Uncaught ValueError: sodium_crypto_pwhash_str(): Argument #3 ($memlimit) must be greater than or equal to %d in %s:%d
Stack trace:
#0 %s(%d): sodium_crypto_pwhash_str()
#1 %s(%d): wrap()
#2 {main}
  thrown in %s on line %d
