--TEST--
Test mcrypt_ecb() function : basic functionality 
--SKIPIF--
<?php 
if (!extension_loaded("mcrypt")) {
	print "skip - mcrypt extension not loaded"; 
}	 
?>
--FILE--
<?php
error_reporting(E_ALL & ~E_DEPRECATED);

/* Prototype  : string mcrypt_ecb(int cipher, string key, string data, int mode, string iv)
 * Description: ECB crypt/decrypt data using key key with cipher cipher starting with iv 
 * Source code: ext/mcrypt/mcrypt.c
 * Alias to functions: 
 */

echo "*** Testing mcrypt_ecb() : basic functionality ***\n";


$cipher = MCRYPT_TRIPLEDES;
$data = b"This is the secret message which must be encrypted";
$mode = MCRYPT_ENCRYPT;

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
   var_dump(bin2hex(mcrypt_ecb($cipher, $key, $data, $mode, $iv)));
}

$key = b"1234567890123456\0\0\0\0\0\0\0\0";
echo "\n--- testing different iv lengths\n";
foreach ($ivs as $iv) {
   echo "\niv length=".strlen($iv)."\n";
   var_dump(bin2hex(mcrypt_ecb($cipher, $key, $data, $mode, $iv)));
}
?>
===DONE===
--EXPECTF--
*** Testing mcrypt_ecb() : basic functionality ***

--- testing different key lengths

key length=8

Warning: mcrypt_ecb(): Key of size 8 not supported by this algorithm. Only keys of size 24 supported in %s on line %d
string(0) ""

key length=20

Warning: mcrypt_ecb(): Key of size 20 not supported by this algorithm. Only keys of size 24 supported in %s on line %d
string(0) ""

key length=24
string(112) "923eedcb20e18e3efa466a6ca1b842b34e6ac46aa3690ef739d0d68a26eb64e1a6ad42e7d18312ae8a57ab927e1dc892e5ff56c061864f27"

key length=26

Warning: mcrypt_ecb(): Key of size 26 not supported by this algorithm. Only keys of size 24 supported in %s on line %d
string(0) ""

--- testing different iv lengths

iv length=4
string(112) "440a6f54601969b15e81df09cd381ef585fede5f3620587fd1a949c520aed9f6d10ebbabf2cea3e1f04c9251c2878c0ca37d51c80d490165"

iv length=8
string(112) "440a6f54601969b15e81df09cd381ef585fede5f3620587fd1a949c520aed9f6d10ebbabf2cea3e1f04c9251c2878c0ca37d51c80d490165"

iv length=9
string(112) "440a6f54601969b15e81df09cd381ef585fede5f3620587fd1a949c520aed9f6d10ebbabf2cea3e1f04c9251c2878c0ca37d51c80d490165"
===DONE===
