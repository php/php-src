--TEST--
Test mcrypt_cbc() function : basic functionality 
--SKIPIF--
<?php 
if (!extension_loaded("mcrypt")) {
	print "skip - mcrypt extension not loaded"; 
}	 
?>
--FILE--
<?php
/* Prototype  : string mcrypt_cbc(int cipher, string key, string data, int mode, string iv)
 * Description: CBC crypt/decrypt data using key key with cipher cipher starting with iv 
 * Source code: ext/mcrypt/mcrypt.c
 * Alias to functions: 
 */

echo "*** Testing mcrypt_cbc() : basic functionality ***\n";


$cipher = MCRYPT_TRIPLEDES;
$data = b"This is the secret message which must be encrypted";
$mode = MCRYPT_ENCRYPT;

// tripledes uses keys upto 192 bits (24 bytes)
$keys = array(
   b'12345678', 
   b'12345678901234567890', 
   b'123456789012345678901234', 
   b'12345678901234567890123456');
// tripledes is a block cipher of 64 bits (8 bytes)
$ivs = array(
   b'1234', 
   b'12345678', 
   b'123456789');


$iv = b'12345678';
echo "\n--- testing different key lengths\n";
foreach ($keys as $key) {
   echo "\nkey length=".strlen($key)."\n";
   var_dump(bin2hex(mcrypt_cbc($cipher, $key, $data, $mode, $iv)));
}

$key = b'1234567890123456';  
echo "\n--- testing different iv lengths\n";
foreach ($ivs as $iv) {
   echo "\niv length=".strlen($iv)."\n";
   var_dump(bin2hex(mcrypt_cbc($cipher, $key, $data, $mode, $iv)));
}
?>
===DONE===
--EXPECTF--
*** Testing mcrypt_cbc() : basic functionality ***

--- testing different key lengths

key length=8

Deprecated: Function mcrypt_cbc() is deprecated in %s on line %d
string(112) "082b437d039d09418e20dc9de1dafa7ed6da5c6335b78950968441da1faf40c1f886e04da8ca177b80b376811e138c1bf51cb48dae2e7939"

key length=20

Deprecated: Function mcrypt_cbc() is deprecated in %s on line %d
string(112) "0627351e0f8a082bf7981ae2c700a43fd3d44b270ac67b00fded1c5796eea935be0fef2a23da0b3f5e243929e62ac957bf0bf463aa90fc4f"

key length=24

Deprecated: Function mcrypt_cbc() is deprecated in %s on line %d
string(112) "b85e21072239d60c63a80e7c9ae493cb741a1cd407e52f451c5f43a0d103f55a7b62617eb2e44213c2d44462d388bc0b8f119384b12c84ac"

key length=26

Deprecated: Function mcrypt_cbc() is deprecated in %s on line %d

Warning: mcrypt_cbc(): Size of key is too large for this algorithm in %s on line %d
string(112) "b85e21072239d60c63a80e7c9ae493cb741a1cd407e52f451c5f43a0d103f55a7b62617eb2e44213c2d44462d388bc0b8f119384b12c84ac"

--- testing different iv lengths

iv length=4

Deprecated: Function mcrypt_cbc() is deprecated in %s on line %d

Warning: mcrypt_cbc(): The IV parameter must be as long as the blocksize in %s on line %d
string(112) "440a6f54601969b127aad3c217ce7583c7f7b29989693130645569301db0020b29a34a3dcd104b2d0e3ba19d6cbd8a33d352b9c27cc34ef1"

iv length=8

Deprecated: Function mcrypt_cbc() is deprecated in %s on line %d
string(112) "bac347506bf092c5557c4363c301745d78f047028e2953e84fd66b30aeb6005812dadbe8baa871b83278341599b0c448ddaaa52b5a378ce5"

iv length=9

Deprecated: Function mcrypt_cbc() is deprecated in %s on line %d

Warning: mcrypt_cbc(): The IV parameter must be as long as the blocksize in %s on line %d
string(112) "440a6f54601969b127aad3c217ce7583c7f7b29989693130645569301db0020b29a34a3dcd104b2d0e3ba19d6cbd8a33d352b9c27cc34ef1"
===DONE===
