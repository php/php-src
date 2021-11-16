--TEST--
Check for libsodium scalarmult
--EXTENSIONS--
sodium
--FILE--
<?php
$n = sodium_hex2bin("5dab087e624a8a4b79e17f8b83800ee66f3bb1292618b6fd1c2f8b27ff88e0eb");
$p = sodium_hex2bin("8520f0098930a754748b7ddcb43ef75a0dbf3a0d26381af4eba4a98eaa9b4e6a");
$q = sodium_crypto_scalarmult($n, $p);

var_dump(sodium_bin2hex($q));
try {
    sodium_crypto_scalarmult(substr($n, 1), $p);
} catch (SodiumException $ex) {
    var_dump(true);
}
?>
--EXPECT--
string(64) "4a5d9d5ba4ce2de1728e3bf480350f25e07e21c947d19e3376f09b3c1e161742"
bool(true)
