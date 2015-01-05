--TEST--
Test mcrypt_encrypt() function : TripleDES functionality 
--SKIPIF--
<?php 
if (!extension_loaded("mcrypt")) {
	print "skip - mcrypt extension not loaded"; 
}	 
?>
--FILE--
<?php
/* Prototype  : string mcrypt_encrypt(string cipher, string key, string data, string mode, string iv)
 * Description: OFB crypt/decrypt data using key key with cipher cipher starting with iv 
 * Source code: ext/mcrypt/mcrypt.c
 * Alias to functions: 
 */

echo "*** Testing mcrypt_encrypt() : TripleDES functionality ***\n";

//test CBC, ECB modes
//test encrypt decrypt
//test tripledes, aes
//test different lengths of key, iv
//test no iv being passed on CBC, ECB 
//test up to 32 bytes with unlimited strength

$cipher = MCRYPT_TRIPLEDES;
$mode = MCRYPT_MODE_CBC;
$data = b'This is the secret message which must be encrypted';

// tripledes uses keys up to 192 bits (24 bytes)
$keys = array(
   b'12345678', 
   b'12345678901234567890', 
   b'123456789012345678901234', 
   b'12345678901234567890123456'
);
// tripledes is a block cipher of 64 bits (8 bytes)
$ivs = array(
   b'1234', 
   b'12345678', 
   b'123456789'
);


$iv = b'12345678';
echo "\n--- testing different key lengths\n";
foreach ($keys as $key) {
   echo "\nkey length=".strlen($key)."\n";
   var_dump(bin2hex(mcrypt_encrypt($cipher, $key, $data, $mode, $iv)));
}

$key = b'123456789012345678901234';
echo "\n--- testing different iv lengths\n";
foreach ($ivs as $iv) {
   echo "\niv length=".strlen($iv)."\n";
   var_dump(bin2hex(mcrypt_encrypt($cipher, $key, $data, $mode, $iv)));
}

?>
===DONE===
--EXPECTF--
*** Testing mcrypt_encrypt() : TripleDES functionality ***

--- testing different key lengths

key length=8

Warning: mcrypt_encrypt(): Key of size 8 not supported by this algorithm. Only keys of size 24 supported in %s on line %d
string(0) ""

key length=20

Warning: mcrypt_encrypt(): Key of size 20 not supported by this algorithm. Only keys of size 24 supported in %s on line %d
string(0) ""

key length=24
string(112) "b85e21072239d60c63a80e7c9ae493cb741a1cd407e52f451c5f43a0d103f55a7b62617eb2e44213c2d44462d388bc0b8f119384b12c84ac"

key length=26

Warning: mcrypt_encrypt(): Key of size 26 not supported by this algorithm. Only keys of size 24 supported in %s on line %d
string(0) ""

--- testing different iv lengths

iv length=4

Warning: mcrypt_encrypt(): Received initialization vector of size 4, but size 8 is required for this encryption mode in %s on line %d
string(0) ""

iv length=8
string(112) "b85e21072239d60c63a80e7c9ae493cb741a1cd407e52f451c5f43a0d103f55a7b62617eb2e44213c2d44462d388bc0b8f119384b12c84ac"

iv length=9

Warning: mcrypt_encrypt(): Received initialization vector of size 9, but size 8 is required for this encryption mode in %s on line %d
string(0) ""
===DONE===
