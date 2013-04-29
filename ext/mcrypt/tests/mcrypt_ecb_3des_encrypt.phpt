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

// tripledes uses keys upto 192 bits (24 bytes)
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

$key = b'1234567890123456';  
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
string(112) "05c9c4cafb9937d950bdae60ee3abcb8d9f3e1f1fac8acaaa5b11d70b7ca02f3b76d447ab3dd85a4b5df5dedb4b4654595ccdf6da97fa93f"

key length=20
string(112) "0fc7045c4fb4dbcf44baf9ed15ab40331a42ff0632318a16b12ed5873f02e7945e4f63f408d6849534cbb7419c22c8854aaa85e0e05a28e6"

key length=24
string(112) "923eedcb20e18e3efa466a6ca1b842b34e6ac46aa3690ef739d0d68a26eb64e1a6ad42e7d18312ae8a57ab927e1dc892e5ff56c061864f27"

key length=26

Warning: mcrypt_ecb(): Size of key is too large for this algorithm in %s on line %d
string(112) "923eedcb20e18e3efa466a6ca1b842b34e6ac46aa3690ef739d0d68a26eb64e1a6ad42e7d18312ae8a57ab927e1dc892e5ff56c061864f27"

--- testing different iv lengths

iv length=4
string(112) "440a6f54601969b15e81df09cd381ef585fede5f3620587fd1a949c520aed9f6d10ebbabf2cea3e1f04c9251c2878c0ca37d51c80d490165"

iv length=8
string(112) "440a6f54601969b15e81df09cd381ef585fede5f3620587fd1a949c520aed9f6d10ebbabf2cea3e1f04c9251c2878c0ca37d51c80d490165"

iv length=9
string(112) "440a6f54601969b15e81df09cd381ef585fede5f3620587fd1a949c520aed9f6d10ebbabf2cea3e1f04c9251c2878c0ca37d51c80d490165"
===DONE===
