--TEST--
Check for libsodium ipcrypt
--EXTENSIONS--
sodium
--SKIPIF--
<?php
if (!defined('SODIUM_CRYPTO_IPCRYPT_KEYBYTES')) print "skip libsodium without ipcrypt (requires >= 1.0.21)";
?>
--FILE--
<?php
$key = str_repeat("\x01", SODIUM_CRYPTO_IPCRYPT_KEYBYTES);

/* Basic encrypt/decrypt roundtrip with IPv4 */
$enc = sodium_crypto_ipcrypt_encrypt("192.168.1.1", $key);
var_dump(sodium_crypto_ipcrypt_decrypt($enc, $key));

/* Deterministic: same input produces same output */
$enc2 = sodium_crypto_ipcrypt_encrypt("192.168.1.1", $key);
var_dump($enc === $enc2);

/* Different key produces different output */
$key2 = str_repeat("\x02", SODIUM_CRYPTO_IPCRYPT_KEYBYTES);
$enc3 = sodium_crypto_ipcrypt_encrypt("192.168.1.1", $key2);
var_dump($enc !== $enc3);

/* IPv6 roundtrip */
$enc6 = sodium_crypto_ipcrypt_encrypt("::1", $key);
var_dump(sodium_crypto_ipcrypt_decrypt($enc6, $key));

/* Keygen produces correct length */
$gen_key = sodium_crypto_ipcrypt_keygen();
var_dump(strlen($gen_key) === SODIUM_CRYPTO_IPCRYPT_KEYBYTES);

/* Error: wrong key length */
try {
    sodium_crypto_ipcrypt_encrypt("192.168.1.1", "short");
} catch (SodiumException $e) {
    echo $e->getMessage() . "\n";
}

/* Error: invalid IP */
try {
    sodium_crypto_ipcrypt_encrypt("not_an_ip", $key);
} catch (SodiumException $e) {
    echo $e->getMessage() . "\n";
}

/* Error: wrong key length for decrypt */
try {
    sodium_crypto_ipcrypt_decrypt("::1", "short");
} catch (SodiumException $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
string(11) "192.168.1.1"
bool(true)
bool(true)
string(3) "::1"
bool(true)
sodium_crypto_ipcrypt_encrypt(): Argument #2 ($key) must be SODIUM_CRYPTO_IPCRYPT_KEYBYTES bytes long
sodium_crypto_ipcrypt_encrypt(): Argument #1 ($ip) must be a valid IP address
sodium_crypto_ipcrypt_decrypt(): Argument #2 ($key) must be SODIUM_CRYPTO_IPCRYPT_KEYBYTES bytes long
