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
error_reporting(E_ALL);

/* Prototype  : string mcrypt_ecb(int cipher, string key, string data, int mode, string iv)
 * Description: ECB crypt/decrypt data using key key with cipher cipher starting with iv 
 * Source code: ext/mcrypt/mcrypt.c
 * Alias to functions: 
 */

$cipher = MCRYPT_TRIPLEDES;
$data = b"This is the secret message which must be encrypted";

// tripledes uses keys up to 192 bits (24 bytes)
$keys = array(
   b'12345678', 
   b'12345678901234567890', 
   b'123456789012345678901234', 
   b'12345678901234567890123456'
);
$data1 = array(
   '0D4ArM3ejyhic9rnCcIW9A==',
   'q0wt1YeOjLpnKm5WsrzKEw==',
   'zwKEFeqHkhlj+7HZTRA/yA==',
   'zwKEFeqHkhlj+7HZTRA/yA=='
);
// tripledes is a block cipher of 64 bits (8 bytes)
$ivs = array(
   b'1234', 
   b'12345678', 
   b'123456789'
);
$data2 = array(
   '+G7nGcWIxigQcJD+2P14HA==',
   '+G7nGcWIxigQcJD+2P14HA==',
   '+G7nGcWIxigQcJD+2P14HA=='
);

$iv = b'12345678';
echo "\n--- testing different key lengths\n";
for ($i = 0; $i < sizeof($keys); $i++) {
   echo "\nkey length=".strlen($keys[$i])."\n";
   special_var_dump(mcrypt_decrypt($cipher, $keys[$i], base64_decode($data1[$i]), MCRYPT_MODE_ECB, $iv));
}

$key = b'123456789012345678901234';
echo "\n--- testing different iv lengths\n";
for ($i = 0; $i < sizeof($ivs); $i++) {
   echo "\niv length=".strlen($ivs[$i])."\n";
   special_var_dump(mcrypt_decrypt($cipher, $key, base64_decode($data2[$i]), MCRYPT_MODE_ECB, $ivs[$i]));
}

function special_var_dump($str) {
   var_dump(bin2hex($str));
}  
?>
===DONE===
--EXPECTF--
--- testing different key lengths

key length=8

Warning: mcrypt_decrypt(): Key of size 8 not supported by this algorithm. Only keys of size 24 supported in %s on line %d
string(0) ""

key length=20

Warning: mcrypt_decrypt(): Key of size 20 not supported by this algorithm. Only keys of size 24 supported in %s on line %d
string(0) ""

key length=24
string(32) "736563726574206d6573736167650000"

key length=26

Warning: mcrypt_decrypt(): Key of size 26 not supported by this algorithm. Only keys of size 24 supported in %s on line %d
string(0) ""

--- testing different iv lengths

iv length=4
string(32) "a9298896ed1b7335f8f10f7ff6d7a239"

iv length=8
string(32) "a9298896ed1b7335f8f10f7ff6d7a239"

iv length=9
string(32) "a9298896ed1b7335f8f10f7ff6d7a239"
===DONE===
