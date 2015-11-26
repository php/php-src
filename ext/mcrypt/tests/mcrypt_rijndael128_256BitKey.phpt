--TEST--
Test mcrypt_encrypt() function : AES functionality 
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
 /* Prototype  : string mcrypt_decrypt(string cipher, string key, string data, string mode, string iv)
 * Description: OFB crypt/decrypt data using key key with cipher cipher starting with iv 
 * Source code: ext/mcrypt/mcrypt.c
 * Alias to functions: 
 */
 /* Prototype  : string mcrypt_cbc(int cipher, string key, string data, int mode, string iv)
 * Description: CBC crypt/decrypt data using key key with cipher cipher starting with iv 
 * Source code: ext/mcrypt/mcrypt.c
 * Alias to functions: 
 */
 /* Prototype  : string mcrypt_ecb(int cipher, string key, string data, int mode, string iv)
 * Description: ECB crypt/decrypt data using key key with cipher cipher starting with iv 
 * Source code: ext/mcrypt/mcrypt.c
 * Alias to functions: 
 */

echo "*** Testing mcrypt : Rijndael128 functionality ***\n";

$cipher = MCRYPT_RIJNDAEL_128;
$mode = MCRYPT_MODE_CBC;
$data = b'This is the secret message which must be encrypted';

// keys : 20 bytes, 24 bytes, 30 Bytes, 32 Bytes, 40 Bytes
$keys = array(
              b'12345678901234567890', 
              b'123456789012345678901234', 
              b'123456789012345678901234567890',
              b'12345678901234567890123456789012',
              b'1234567890123456789012345678901234567890'
 );
// rijndael128 is a block cipher of 128 bits (16 bytes)
$iv = b'1234567890123456';

echo "\n--- testing different key lengths\n";
foreach ($keys as $key) {
   echo "\nkey length=".strlen($key)."\n";
   $res = mcrypt_encrypt($cipher, $key, $data, MCRYPT_MODE_CBC, $iv);
   var_dump(bin2hex($res));
   $res = mcrypt_decrypt($cipher, $key, $res, MCRYPT_MODE_CBC, $iv);
   var_dump(bin2hex($res));   
}
?>
===DONE===
--EXPECTF--
*** Testing mcrypt : Rijndael128 functionality ***

--- testing different key lengths

key length=20

Warning: mcrypt_encrypt(): Key of size 20 not supported by this algorithm. Only keys of sizes 16, 24 or 32 supported in %s on line %d
string(0) ""

Warning: mcrypt_decrypt(): Key of size 20 not supported by this algorithm. Only keys of sizes 16, 24 or 32 supported in %s on line %d
string(0) ""

key length=24
string(128) "8ecdf1ed5742aff16ef34c819c8d22c707c54f4d9ffc18e5f6ab79fe68c25705351e2c001a0b9f29e5def67570ca9da644efb69a8bb97940cb4bec094dae8bb5"
string(128) "546869732069732074686520736563726574206d657373616765207768696368206d75737420626520656e637279707465640000000000000000000000000000"

key length=30

Warning: mcrypt_encrypt(): Key of size 30 not supported by this algorithm. Only keys of sizes 16, 24 or 32 supported in %s on line %d
string(0) ""

Warning: mcrypt_decrypt(): Key of size 30 not supported by this algorithm. Only keys of sizes 16, 24 or 32 supported in %s on line %d
string(0) ""

key length=32
string(128) "f23bc103bfd0859a8318acee6d96e5f43dff68f3cdeae817a1e77c33492e32bdb82c5f660fcd1a2bfda70d9de4d5d8028ce179a9e2f7f9ee7dd61c7b4b409e95"
string(128) "546869732069732074686520736563726574206d657373616765207768696368206d75737420626520656e637279707465640000000000000000000000000000"

key length=40

Warning: mcrypt_encrypt(): Key of size 40 not supported by this algorithm. Only keys of sizes 16, 24 or 32 supported in %s on line %d
string(0) ""

Warning: mcrypt_decrypt(): Key of size 40 not supported by this algorithm. Only keys of sizes 16, 24 or 32 supported in %s on line %d
string(0) ""
===DONE===
