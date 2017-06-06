--TEST--
Check for libsodium shorthash
--SKIPIF--
<?php if (!extension_loaded("sodium")) print "skip"; ?>
--FILE--
<?php
$m1 = 'message';
$k1 = '0123456789ABCDEF';
$h1 = sodium_crypto_shorthash($m1, $k1);
echo bin2hex($h1) . "\n";
$k2 = '0123456789abcdef';
$h2 = sodium_crypto_shorthash($m1, $k2);
echo bin2hex($h2) . "\n";
$m2 = 'msg';
$h3 = sodium_crypto_shorthash($m2, $k2);
echo bin2hex($h3) . "\n";

try {
	sodium_crypto_shorthash($m1, $k1 . $k2);
} catch (SodiumException $ex) {
	var_dump(true);
}
?>
--EXPECT--
e0ad6fdbf8b9a191
c667b37af201a2d9
d27fa3fc70b45b72
bool(true)
