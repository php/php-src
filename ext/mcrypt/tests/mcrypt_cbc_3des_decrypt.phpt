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

$cipher = MCRYPT_TRIPLEDES;
$data = b"This is the secret message which must be encrypted";

// tripledes uses keys with exactly 192 bits (24 bytes)
$keys = array(
   b'12345678', 
   b'12345678901234567890', 
   b'123456789012345678901234', 
   b'12345678901234567890123456'
);
$data1 = array(
   'IleMhoxiOthmHua4tFBHOw==',
   'EeF1s6C+w1IiHj1gdDn81g==',
   'EEuXpjZPueyYoG0LGQ199Q==',
   'EEuXpjZPueyYoG0LGQ199Q=='
);
// tripledes is a block cipher of 64 bits (8 bytes)
$ivs = array(
   b'1234', 
   b'12345678', 
   b'123456789'
);
   // data represented in base64 (ascii)
$data2 = array(
   '+G7nGcWIxij3TZjpI9lJdQ==',
   '3bJiFMeyScxOLQcE6mZtLg==',
   '+G7nGcWIxij3TZjpI9lJdQ=='
);

$iv = b'12345678';
echo "\n--- testing different key lengths\n";
for ($i = 0; $i < sizeof($keys); $i++) {
   echo "\nkey length=".strlen($keys[$i])."\n";
   special_var_dump(mcrypt_decrypt($cipher, $keys[$i], base64_decode($data1[$i]), MCRYPT_MODE_CBC, $iv));
}

$key = b'123456789012345678901234';  
echo "\n--- testing different iv lengths\n";
for ($i = 0; $i < sizeof($ivs); $i++) {
   echo "\niv length=".strlen($ivs[$i])."\n";
   special_var_dump(mcrypt_decrypt($cipher, $key, base64_decode($data2[$i]), MCRYPT_MODE_CBC, $ivs[$i]));
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

Warning: mcrypt_decrypt(): Received initialization vector of size 4, but size 8 is required for this encryption mode in %s on line %d
string(0) ""

iv length=8
string(32) "659ec947f4dc3a3b9c50de744598d3c8"

iv length=9

Warning: mcrypt_decrypt(): Received initialization vector of size 9, but size 8 is required for this encryption mode in %s on line %d
string(0) ""
===DONE===
