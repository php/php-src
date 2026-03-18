--TEST--
Check for libsodium XOF SHAKE128
--EXTENSIONS--
sodium
--SKIPIF--
<?php
if (!defined('SODIUM_CRYPTO_XOF_SHAKE128_STATEBYTES')) print "skip libsodium without XOF support (requires >= 1.0.21)";
?>
--FILE--
<?php
/* One-shot: NIST test vector SHAKE128("") */
$out = sodium_crypto_xof_shake128(32, "");
var_dump(bin2hex($out));

/* One-shot: SHAKE128("abc") */
$out = sodium_crypto_xof_shake128(32, "abc");
var_dump(bin2hex($out));

/* Variable output length */
$out16 = sodium_crypto_xof_shake128(16, "abc");
$out64 = sodium_crypto_xof_shake128(64, "abc");
var_dump(strlen($out16));
var_dump(strlen($out64));
var_dump(bin2hex($out16) === substr(bin2hex($out64), 0, 32));

/* Streaming matches one-shot */
$state = sodium_crypto_xof_shake128_init();
sodium_crypto_xof_shake128_update($state, "abc");
$sq = sodium_crypto_xof_shake128_squeeze($state, 32);
var_dump(bin2hex($sq));

/* Multi-part update matches one-shot */
$state = sodium_crypto_xof_shake128_init();
sodium_crypto_xof_shake128_update($state, "a");
sodium_crypto_xof_shake128_update($state, "bc");
$sq = sodium_crypto_xof_shake128_squeeze($state, 32);
var_dump(bin2hex($sq));

/* Multiple squeezes concatenated match single squeeze */
$state = sodium_crypto_xof_shake128_init();
sodium_crypto_xof_shake128_update($state, "abc");
$sq1 = sodium_crypto_xof_shake128_squeeze($state, 16);
$sq2 = sodium_crypto_xof_shake128_squeeze($state, 16);
var_dump(bin2hex($sq1 . $sq2));

/* Domain separation produces different output */
$state_d = sodium_crypto_xof_shake128_init(0x42);
sodium_crypto_xof_shake128_update($state_d, "abc");
$sq_d = sodium_crypto_xof_shake128_squeeze($state_d, 32);
var_dump(bin2hex($sq_d));
var_dump(bin2hex($sq_d) !== bin2hex($sq));

/* Error: zero length */
try {
    sodium_crypto_xof_shake128(0, "test");
} catch (SodiumException $e) {
    echo $e->getMessage() . "\n";
}

/* Error: negative length */
try {
    sodium_crypto_xof_shake128(-1, "test");
} catch (SodiumException $e) {
    echo $e->getMessage() . "\n";
}

/* Error: domain out of range (0x00) */
try {
    sodium_crypto_xof_shake128_init(0);
} catch (SodiumException $e) {
    echo $e->getMessage() . "\n";
}

/* Error: domain out of range (0x80) */
try {
    sodium_crypto_xof_shake128_init(0x80);
} catch (SodiumException $e) {
    echo $e->getMessage() . "\n";
}

/* Error: bad state */
try {
    $bad = "not_a_state";
    sodium_crypto_xof_shake128_update($bad, "test");
} catch (SodiumException $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
string(64) "7f9c2ba4e88f827d616045507605853ed73b8093f6efbc88eb1a6eacfa66ef26"
string(64) "5881092dd818bf5cf8a3ddb793fbcba74097d5c526a6d35f97b83351940f2cc8"
int(16)
int(64)
bool(true)
string(64) "5881092dd818bf5cf8a3ddb793fbcba74097d5c526a6d35f97b83351940f2cc8"
string(64) "5881092dd818bf5cf8a3ddb793fbcba74097d5c526a6d35f97b83351940f2cc8"
string(64) "5881092dd818bf5cf8a3ddb793fbcba74097d5c526a6d35f97b83351940f2cc8"
string(64) "f3a3a89c329e644a7d2351744d9a28c953698b64102e912085ce1f6d79fa311e"
bool(true)
sodium_crypto_xof_shake128(): Argument #1 ($length) must be a positive integer
sodium_crypto_xof_shake128(): Argument #1 ($length) must be a positive integer
sodium_crypto_xof_shake128_init(): Argument #1 ($domain) must be between 0x01 and 0x7f
sodium_crypto_xof_shake128_init(): Argument #1 ($domain) must be between 0x01 and 0x7f
sodium_crypto_xof_shake128_update(): Argument #1 ($state) must have a correct state length
