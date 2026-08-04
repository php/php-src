--TEST--
sodium_crypto_pwhash(): a below-minimum memlimit reports a precise argument error
--EXTENSIONS--
sodium
--SKIPIF--
<?php
if (!defined('SODIUM_CRYPTO_PWHASH_SALTBYTES')) print "skip libsodium without argon2";
?>
--FILE--
<?php
$salt = str_repeat("a", SODIUM_CRYPTO_PWHASH_SALTBYTES);

try {
    sodium_crypto_pwhash(32, "password", $salt, SODIUM_CRYPTO_PWHASH_OPSLIMIT_INTERACTIVE, 1);
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    sodium_crypto_pwhash_str("password", SODIUM_CRYPTO_PWHASH_OPSLIMIT_INTERACTIVE, 1);
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
sodium_crypto_pwhash(): Argument #5 ($memlimit) must be greater than or equal to %d
sodium_crypto_pwhash_str(): Argument #3 ($memlimit) must be greater than or equal to %d
