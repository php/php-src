--TEST--
Check for libsodium bin2hex
--SKIPIF--
<?php if (!extension_loaded("sodium")) print "skip"; ?>
--FILE--
<?php
$bin = random_bytes(random_int(1, 1000));
$hex = sodium_bin2hex($bin);
$phphex = bin2hex($bin);
var_dump(strcasecmp($hex, $phphex));

$bin2 = sodium_hex2bin($hex);
var_dump($bin2 === $bin);

$bin2 = sodium_hex2bin('[' . $hex .']', '[]');
var_dump($bin2 === $bin);
?>
--EXPECT--
int(0)
bool(true)
bool(true)
