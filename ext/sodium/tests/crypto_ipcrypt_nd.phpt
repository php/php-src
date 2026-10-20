--TEST--
Check for libsodium ipcrypt non-deterministic (nd) and extended (ndx)
--EXTENSIONS--
sodium
--SKIPIF--
<?php
if (!defined('SODIUM_CRYPTO_IPCRYPT_KEYBYTES')) print "skip libsodium without ipcrypt (requires >= 1.0.21)";
?>
--FILE--
<?php
/* ND: non-deterministic encrypt/decrypt roundtrip */
$nd_key = sodium_crypto_ipcrypt_nd_keygen();
var_dump(strlen($nd_key) === SODIUM_CRYPTO_IPCRYPT_ND_KEYBYTES);

$ct = sodium_crypto_ipcrypt_nd_encrypt("192.168.1.1", $nd_key);
var_dump(strlen($ct) === SODIUM_CRYPTO_IPCRYPT_ND_OUTPUTBYTES * 2);
$pt = sodium_crypto_ipcrypt_nd_decrypt($ct, $nd_key);
var_dump($pt);

/* ND is non-deterministic: two encryptions of the same IP differ */
$ct2 = sodium_crypto_ipcrypt_nd_encrypt("192.168.1.1", $nd_key);
var_dump($ct !== $ct2);
$pt2 = sodium_crypto_ipcrypt_nd_decrypt($ct2, $nd_key);
var_dump($pt2);

/* ND: IPv6 roundtrip */
$ct6 = sodium_crypto_ipcrypt_nd_encrypt("::1", $nd_key);
var_dump(sodium_crypto_ipcrypt_nd_decrypt($ct6, $nd_key));

/* ND error: wrong key length */
try {
    sodium_crypto_ipcrypt_nd_encrypt("192.168.1.1", "short");
} catch (SodiumException $e) {
    echo $e->getMessage() . "\n";
}

/* ND error: invalid IP */
try {
    sodium_crypto_ipcrypt_nd_encrypt("bad", $nd_key);
} catch (SodiumException $e) {
    echo $e->getMessage() . "\n";
}

/* ND error: bad ciphertext hex length */
try {
    sodium_crypto_ipcrypt_nd_decrypt("tooshort", $nd_key);
} catch (SodiumException $e) {
    echo $e->getMessage() . "\n";
}

/* NDX: keygen, encrypt, decrypt roundtrip */
$ndx_key = sodium_crypto_ipcrypt_ndx_keygen();
var_dump(strlen($ndx_key) === SODIUM_CRYPTO_IPCRYPT_NDX_KEYBYTES);

$ct_ndx = sodium_crypto_ipcrypt_ndx_encrypt("10.0.0.1", $ndx_key);
var_dump(strlen($ct_ndx) === SODIUM_CRYPTO_IPCRYPT_NDX_OUTPUTBYTES * 2);
$pt_ndx = sodium_crypto_ipcrypt_ndx_decrypt($ct_ndx, $ndx_key);
var_dump($pt_ndx);

/* NDX is non-deterministic */
$ct_ndx2 = sodium_crypto_ipcrypt_ndx_encrypt("10.0.0.1", $ndx_key);
var_dump($ct_ndx !== $ct_ndx2);

/* NDX: IPv6 */
$ct_ndx6 = sodium_crypto_ipcrypt_ndx_encrypt("fe80::1", $ndx_key);
var_dump(sodium_crypto_ipcrypt_ndx_decrypt($ct_ndx6, $ndx_key));

/* NDX error: wrong key length */
try {
    sodium_crypto_ipcrypt_ndx_encrypt("10.0.0.1", "short");
} catch (SodiumException $e) {
    echo $e->getMessage() . "\n";
}

/* NDX error: bad ciphertext hex */
try {
    sodium_crypto_ipcrypt_ndx_decrypt("tooshort", $ndx_key);
} catch (SodiumException $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
bool(true)
bool(true)
string(11) "192.168.1.1"
bool(true)
string(11) "192.168.1.1"
string(3) "::1"
sodium_crypto_ipcrypt_nd_encrypt(): Argument #2 ($key) must be SODIUM_CRYPTO_IPCRYPT_ND_KEYBYTES bytes long
sodium_crypto_ipcrypt_nd_encrypt(): Argument #1 ($ip) must be a valid IP address
sodium_crypto_ipcrypt_nd_decrypt(): Argument #1 ($ciphertext_hex) must be a valid hex-encoded ciphertext
bool(true)
bool(true)
string(8) "10.0.0.1"
bool(true)
string(7) "fe80::1"
sodium_crypto_ipcrypt_ndx_encrypt(): Argument #2 ($key) must be SODIUM_CRYPTO_IPCRYPT_NDX_KEYBYTES bytes long
sodium_crypto_ipcrypt_ndx_decrypt(): Argument #1 ($ciphertext_hex) must be a valid hex-encoded ciphertext
