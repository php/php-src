--TEST--
Check for libsodium XOF TurboSHAKE128 and TurboSHAKE256
--EXTENSIONS--
sodium
--SKIPIF--
<?php
if (!defined('SODIUM_CRYPTO_XOF_TURBOSHAKE128_STATEBYTES')) print "skip libsodium without XOF support (requires >= 1.0.21)";
?>
--FILE--
<?php
/* TurboSHAKE128 one-shot */
$out = sodium_crypto_xof_turboshake128(32, "");
var_dump(bin2hex($out));
$out = sodium_crypto_xof_turboshake128(32, "abc");
var_dump(bin2hex($out));

/* TurboSHAKE256 one-shot */
$out = sodium_crypto_xof_turboshake256(32, "");
var_dump(bin2hex($out));
$out = sodium_crypto_xof_turboshake256(32, "abc");
var_dump(bin2hex($out));

/* TurboSHAKE128 streaming matches one-shot */
$state = sodium_crypto_xof_turboshake128_init();
sodium_crypto_xof_turboshake128_update($state, "abc");
$sq = sodium_crypto_xof_turboshake128_squeeze($state, 32);
var_dump(bin2hex($sq));

/* TurboSHAKE256 streaming matches one-shot */
$state = sodium_crypto_xof_turboshake256_init();
sodium_crypto_xof_turboshake256_update($state, "abc");
$sq = sodium_crypto_xof_turboshake256_squeeze($state, 32);
var_dump(bin2hex($sq));

/* TurboSHAKE128 multi-part update */
$state = sodium_crypto_xof_turboshake128_init();
sodium_crypto_xof_turboshake128_update($state, "a");
sodium_crypto_xof_turboshake128_update($state, "bc");
$sq = sodium_crypto_xof_turboshake128_squeeze($state, 32);
var_dump(bin2hex($sq));

/* TurboSHAKE256 multi-part update */
$state = sodium_crypto_xof_turboshake256_init();
sodium_crypto_xof_turboshake256_update($state, "a");
sodium_crypto_xof_turboshake256_update($state, "bc");
$sq = sodium_crypto_xof_turboshake256_squeeze($state, 32);
var_dump(bin2hex($sq));

/* TurboSHAKE128 domain separation */
$state_d = sodium_crypto_xof_turboshake128_init(0x42);
sodium_crypto_xof_turboshake128_update($state_d, "abc");
$sq_d = sodium_crypto_xof_turboshake128_squeeze($state_d, 32);
var_dump(bin2hex($sq_d));

/* TurboSHAKE256 domain separation */
$state_d = sodium_crypto_xof_turboshake256_init(0x42);
sodium_crypto_xof_turboshake256_update($state_d, "abc");
$sq_d = sodium_crypto_xof_turboshake256_squeeze($state_d, 32);
var_dump(bin2hex($sq_d));

/* Variable output length */
$out16 = sodium_crypto_xof_turboshake128(16, "abc");
$out64 = sodium_crypto_xof_turboshake128(64, "abc");
var_dump(strlen($out16));
var_dump(strlen($out64));
var_dump(bin2hex($out16) === substr(bin2hex($out64), 0, 32));

/* TurboSHAKE128 error: zero length */
try {
    sodium_crypto_xof_turboshake128(0, "test");
} catch (SodiumException $e) {
    echo $e->getMessage() . "\n";
}

/* TurboSHAKE256 error: bad state */
try {
    $bad = "not_a_state";
    sodium_crypto_xof_turboshake256_update($bad, "test");
} catch (SodiumException $e) {
    echo $e->getMessage() . "\n";
}

/* TurboSHAKE128 error: domain out of range */
try {
    sodium_crypto_xof_turboshake128_init(0);
} catch (SodiumException $e) {
    echo $e->getMessage() . "\n";
}
try {
    sodium_crypto_xof_turboshake256_init(0x80);
} catch (SodiumException $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
string(64) "1e415f1c5983aff2169217277d17bb538cd945a397ddec541f1ce41af2c1b74c"
string(64) "dcf1646dfe993a8eb6b782d1faaca6d82416a5dcf1de98ee3c6dbc5e1dc63018"
string(64) "367a329dafea871c7802ec67f905ae13c57695dc2c6663c61035f59a18f8e7db"
string(64) "63824b1431a7372e85edc022c9d7afdd027472fcfa33c887d6f5aaf8dc5d4db6"
string(64) "dcf1646dfe993a8eb6b782d1faaca6d82416a5dcf1de98ee3c6dbc5e1dc63018"
string(64) "63824b1431a7372e85edc022c9d7afdd027472fcfa33c887d6f5aaf8dc5d4db6"
string(64) "dcf1646dfe993a8eb6b782d1faaca6d82416a5dcf1de98ee3c6dbc5e1dc63018"
string(64) "63824b1431a7372e85edc022c9d7afdd027472fcfa33c887d6f5aaf8dc5d4db6"
string(64) "3f7566fb02630888cba2af090aaf544ac6e85484d8662335b0c0fd6b3c83ea1b"
string(64) "0137a7e8cca0b99e9bdd557a7caac1f21d65bc2a2ccbde1e1f8c702352a2bf30"
int(16)
int(64)
bool(true)
sodium_crypto_xof_turboshake128(): Argument #1 ($length) must be a positive integer
sodium_crypto_xof_turboshake256_update(): Argument #1 ($state) must have a correct state length
sodium_crypto_xof_turboshake128_init(): Argument #1 ($domain) must be between 0x01 and 0x7f
sodium_crypto_xof_turboshake256_init(): Argument #1 ($domain) must be between 0x01 and 0x7f
