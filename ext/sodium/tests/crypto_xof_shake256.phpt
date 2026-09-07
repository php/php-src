--TEST--
Check for libsodium XOF SHAKE256
--EXTENSIONS--
sodium
--SKIPIF--
<?php
if (!defined('SODIUM_CRYPTO_XOF_SHAKE256_STATEBYTES')) print "skip libsodium without XOF support (requires >= 1.0.21)";
?>
--FILE--
<?php
/* One-shot: NIST test vector SHAKE256("") */
$out = sodium_crypto_xof_shake256(32, "");
var_dump(bin2hex($out));

/* One-shot: SHAKE256("abc") */
$out = sodium_crypto_xof_shake256(32, "abc");
var_dump(bin2hex($out));

/* Variable output length */
$out16 = sodium_crypto_xof_shake256(16, "abc");
$out64 = sodium_crypto_xof_shake256(64, "abc");
var_dump(strlen($out16));
var_dump(strlen($out64));
var_dump(bin2hex($out16) === substr(bin2hex($out64), 0, 32));

/* Streaming matches one-shot */
$state = sodium_crypto_xof_shake256_init();
sodium_crypto_xof_shake256_update($state, "abc");
$sq = sodium_crypto_xof_shake256_squeeze($state, 32);
var_dump(bin2hex($sq));

/* Multi-part update */
$state = sodium_crypto_xof_shake256_init();
sodium_crypto_xof_shake256_update($state, "a");
sodium_crypto_xof_shake256_update($state, "bc");
$sq = sodium_crypto_xof_shake256_squeeze($state, 32);
var_dump(bin2hex($sq));

/* Domain separation */
$state_d = sodium_crypto_xof_shake256_init(0x42);
sodium_crypto_xof_shake256_update($state_d, "abc");
$sq_d = sodium_crypto_xof_shake256_squeeze($state_d, 32);
var_dump(bin2hex($sq_d));

/* Error: zero length */
try {
    sodium_crypto_xof_shake256(0, "test");
} catch (SodiumException $e) {
    echo $e->getMessage() . "\n";
}

/* Error: bad state */
try {
    $bad = "not_a_state";
    sodium_crypto_xof_shake256_update($bad, "test");
} catch (SodiumException $e) {
    echo $e->getMessage() . "\n";
}

/* Error: squeeze bad length */
try {
    $state = sodium_crypto_xof_shake256_init();
    sodium_crypto_xof_shake256_update($state, "abc");
    sodium_crypto_xof_shake256_squeeze($state, 0);
} catch (SodiumException $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
string(64) "46b9dd2b0ba88d13233b3feb743eeb243fcd52ea62b81b82b50c27646ed5762f"
string(64) "483366601360a8771c6863080cc4114d8db44530f8f1e1ee4f94ea37e78b5739"
int(16)
int(64)
bool(true)
string(64) "483366601360a8771c6863080cc4114d8db44530f8f1e1ee4f94ea37e78b5739"
string(64) "483366601360a8771c6863080cc4114d8db44530f8f1e1ee4f94ea37e78b5739"
string(64) "1259d63c872d50fee4500685419f489966971c0a77e2058fab0e48dfb12d24bc"
sodium_crypto_xof_shake256(): Argument #1 ($length) must be a positive integer
sodium_crypto_xof_shake256_update(): Argument #1 ($state) must have a correct state length
sodium_crypto_xof_shake256_squeeze(): Argument #2 ($length) must be a positive integer
