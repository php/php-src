--TEST--
Check for libsodium core ristretto255
--EXTENSIONS--
sodium
--SKIPIF--
<?php
if (!defined('SODIUM_CRYPTO_CORE_RISTRETTO255_HASHBYTES')) print "skip libsodium without Ristretto255";
?>
--FILE--
<?php
$badHex = array(
    /* Non-canonical field encodings */
    "00ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff",
    "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff7f",
    "f3ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff7f",
    "edffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff7f",
    "0100000000000000000000000000000000000000000000000000000000000080",

    /* Negative field elements */
    "0100000000000000000000000000000000000000000000000000000000000000",
    "01ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff7f",
    "ed57ffd8c914fb201471d1c3d245ce3c746fcbe63a3679d51b6a516ebebe0e20",
    "c34c4e1826e5d403b78e246e88aa051c36ccf0aafebffe137d148a2bf9104562",
    "c940e5a4404157cfb1628b108db051a8d439e1a421394ec4ebccb9ec92a8ac78",
    "47cfc5497c53dc8e61c91d17fd626ffb1c49e2bca94eed052281b510b1117a24",
    "f1c6165d33367351b0da8f6e4511010c68174a03b6581212c71c0e1d026c3c72",
    "87260f7a2f12495118360f02c26a470f450dadf34a413d21042b43b9d93e1309",

    /* Non-square x^2 */
    "26948d35ca62e643e26a83177332e6b6afeb9d08e4268b650f1f5bbd8d81d371",
    "4eac077a713c57b4f4397629a4145982c661f48044dd3f96427d40b147d9742f",
    "de6a7b00deadc788eb6b6c8d20c0ae96c2f2019078fa604fee5b87d6e989ad7b",
    "bcab477be20861e01e4a0e295284146a510150d9817763caf1a6f4b422d67042",
    "2a292df7e32cababbd9de088d1d1abec9fc0440f637ed2fba145094dc14bea08",
    "f4a9e534fc0d216c44b218fa0c42d99635a0127ee2e53c712f70609649fdff22",
    "8268436f8c4126196cf64b3c7ddbda90746a378625f9813dd9b8457077256731",
    "2810e5cbc2cc4d4eece54f61c6f69758e289aa7ab440b3cbeaa21995c2f4232b",

    /* Negative xy value */
    "3eb858e78f5a7254d8c9731174a94f76755fd3941c0ac93735c07ba14579630e",
    "a45fdc55c76448c049a1ab33f17023edfb2be3581e9c7aade8a6125215e04220",
    "d483fe813c6ba647ebbfd3ec41adca1c6130c2beeee9d9bf065c8d151c5f396e",
    "8a2e1d30050198c65a54483123960ccc38aef6848e1ec8f5f780e8523769ba32",
    "32888462f8b486c68ad7dd9610be5192bbeaf3b443951ac1a8118419d9fa097b",
    "227142501b9d4355ccba290404bde41575b037693cef1f438c47f8fbf35d1165",
    "5c37cc491da847cfeb9281d407efc41e15144c876e0170b499a96a22ed31e01e",
    "445425117cb8c90edcbc7c1cc0e74f747f2c1efa5630a967c64f287792a48a4b",

    /* s = -1, which causes y = 0 */
    "ecffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff7f"
);

// First 16 lines of output: non-canonical encodings are rejected
for ($i = 0; $i < count($badHex); ++$i) {
    $s = sodium_hex2bin($badHex[$i]);
    var_dump(sodium_crypto_core_ristretto255_is_valid_point($s));
}

$inputHash = sodium_hex2bin(
    "5d1be09e3d0c82fc538112490e35701979d99e06ca3e2b5b54bffe8b4dc772c1" .
    "4d98b696a1bbfb5ca32c436cc61c16563790306c79eaca7705668b47dffe5bb6"
);
$outHash = sodium_crypto_core_ristretto255_from_hash($inputHash);
var_dump(sodium_bin2hex($outHash));

// Any random scalar should be valid, and so too should (r * basepoint)
$r = sodium_crypto_core_ristretto255_scalar_random();
$s0 = sodium_crypto_scalarmult_ristretto255_base($r);
var_dump(sodium_crypto_core_ristretto255_is_valid_point($s0));

// Test that multiplying by the order of the curve fails:
$L = "\xed\xd3\xf5\x5c\x1a\x63\x12\x58" .
     "\xd6\x9c\xf7\xa2\xde\xf9\xde\x14" .
     "\x00\x00\x00\x00\x00\x00\x00\x00" .
     "\x00\x00\x00\x00\x00\x00\x00\x10";

$s = sodium_crypto_core_ristretto255_random();
try {
    $multL = sodium_crypto_scalarmult_ristretto255($s, $L);
} catch (SodiumException $e) {
    echo $e->getMessage(), "\n";
}
$s2 = sodium_crypto_scalarmult_ristretto255($r, $s);

// _from_hash should produce a valid point
$ru = random_bytes(64);
$s3 = sodium_crypto_core_ristretto255_from_hash($ru);
var_dump(sodium_crypto_core_ristretto255_is_valid_point($s3));

// Modular inverse should be valid too (???)
//$r_invert = sodium_crypto_core_ristretto255_scalar_invert($r);
//var_dump(sodium_crypto_core_ristretto255_is_valid_point($r_invert));

$s_plus = sodium_crypto_core_ristretto255_add($s, $s0);
$s_minus = sodium_crypto_core_ristretto255_sub($s_plus, $s0);
var_dump(hash_equals($s, $s_minus));
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
string(64) "3066f82a1a747d45120d1740f14358531a8f04bbffe6a819f86dfe50f44a0a46"
bool(true)
Result is identity element
bool(true)
bool(true)
