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
/* Prototype  : string mcrypt_ecb(int cipher, string key, string data, int mode, string iv)
 * Description: ECB crypt/decrypt data using key key with cipher cipher starting with iv 
 * Source code: ext/mcrypt/mcrypt.c
 * Alias to functions: 
 */

echo "*** Testing mcrypt_ecb() : basic functionality ***\n";


$cipher = MCRYPT_TRIPLEDES;
$data = b"This is the secret message which must be encrypted";
$mode = MCRYPT_DECRYPT;

// tripledes uses keys upto 192 bits (24 bytes)
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
   special_var_dump(mcrypt_ecb($cipher, $keys[$i], base64_decode($data1[$i]), $mode, $iv));
}

$key = b'1234567890123456';  
echo "\n--- testing different iv lengths\n";
for ($i = 0; $i < sizeof($ivs); $i++) {
   echo "\niv length=".strlen($ivs[$i])."\n";
   special_var_dump(mcrypt_ecb($cipher, $key, base64_decode($data2[$i]), $mode, $ivs[$i]));
}

function special_var_dump($str) {
   var_dump(bin2hex($str));
}  
?>
===DONE===
--EXPECTF--
*** Testing mcrypt_ecb() : basic functionality ***

--- testing different key lengths

key length=8
string(32) "736563726574206d6573736167650000"

key length=20
string(32) "736563726574206d6573736167650000"

key length=24
string(32) "736563726574206d6573736167650000"

key length=26

Warning: mcrypt_ecb(): Size of key is too large for this algorithm in %s on line %d
string(32) "736563726574206d6573736167650000"

--- testing different iv lengths

iv length=4
string(32) "736563726574206d6573736167650000"

iv length=8
string(32) "736563726574206d6573736167650000"

iv length=9
string(32) "736563726574206d6573736167650000"
===DONE===