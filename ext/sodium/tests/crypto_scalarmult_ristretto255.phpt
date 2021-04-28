--TEST--
Check for libsodium scalarmult ristretto255
--EXTENSIONS--
sodium
--SKIPIF--
<?php
if (!defined('SODIUM_CRYPTO_SCALARMULT_RISTRETTO255_HASHBYTES')) print "skip libsodium without Ristretto255";
?>
--FILE--
<?php
$n = sodium_hex2bin("94938bc8631c7d760f6a8b9d9c9c07569e65d9cf79dc809221186205fea3ec05");
$p = sodium_hex2bin("edf2014b8a2ca9ec18e3ba4600c3c9c48d38acebba01601ad7b104a492035b06");
$q = sodium_crypto_scalarmult_ristretto255($n, $p);
$q2 = sodium_crypto_scalarmult_ristretto255_base($n);

var_dump(sodium_bin2hex($q));
var_dump(sodium_bin2hex($q2));
try {
    sodium_crypto_scalarmult(substr($n, 1), $p);
} catch (SodiumException $ex) {
    var_dump(true);
}
?>
--EXPECT--
string(64) "2a684afd8de19c6964fffd28509294e2752fdbb79e13a58dec3aff51de65505e"
string(64) "e08ec8d22c0901c1746da3844857e9bc25b77cfe14a412e7bcd2b4017aff0556"
bool(true)
