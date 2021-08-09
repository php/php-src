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
$k = sodium_hex2bin("71a330faff41651c6dfa6e4548877d2dc2b0c26056c2e7e17bfb14cf94a4b47c");
$a = sodium_hex2bin("92d753c7b3fef8b8b553e672823db0a052d7598999a3baacd5909f0c0a6d491f");
$b = sodium_crypto_scalarmult_ristretto255($k, $a);
var_dump(sodium_bin2hex($b));
?>
--EXPECT--
string(64) "2a684afd8de19c6964fffd28509294e2752fdbb79e13a58dec3aff51de65505e"
string(64) "e08ec8d22c0901c1746da3844857e9bc25b77cfe14a412e7bcd2b4017aff0556"
bool(true)
string(64) "188101d76706b149e38523f7100891d0cc83e784d8499ca4899a9fe3a194ae6a"
