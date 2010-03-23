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

echo "*** Testing mcrypt : Rijndael128 functionality ***\n";

$cipher = MCRYPT_RIJNDAEL_128;
$mode = MCRYPT_MODE_CBC;
$data = b'This is the secret message which must be encrypted';

// keys upto 128 bits (16 bytes)
$keys = array(
   null, 
   '', 
   b'12345678', 
   b'1234567890123456'
);
// rijndael128 is a block cipher of 128 bits (16 bytes)
$ivs = array(
   null, 
   '', 
   b'12345678', 
   b'1234567890123456', 
   b'12345678901234567'
);


$iv = b'1234567890123456';
echo "\n--- testing different key lengths\n";
foreach ($keys as $key) {
   echo "\nkey length=".strlen($key)."\n";
   $res = mcrypt_encrypt($cipher, $key, $data, MCRYPT_MODE_CBC, $iv);
   var_dump(bin2hex($res));
   $res = mcrypt_cbc($cipher, $key, $res, MCRYPT_DECRYPT, $iv);
   var_dump(bin2hex($res));   
}

$key = b'1234567890123456';  
echo "\n--- testing different iv lengths\n";
foreach ($ivs as $iv) {
   echo "\niv length=".strlen($iv)."\n";
   $res = mcrypt_cbc($cipher, $key, $data, $mode, $iv);
   var_dump(bin2hex($res));
   $res = mcrypt_decrypt($cipher, $key, $res, MCRYPT_MODE_CBC, $iv);
   var_dump(bin2hex($res));   
}

?>
===DONE===
--EXPECTF--
*** Testing mcrypt : Rijndael128 functionality ***

--- testing different key lengths

key length=0
string(128) "4fbf24aaa789f5194260ade1acd9499402c1845cc517e8fe43cfb5b90a0df294db33ecd1a836c47d6bf6d8600512ba415e17008a1e1991f81056258d82099397"
string(128) "546869732069732074686520736563726574206d657373616765207768696368206d75737420626520656e637279707465640000000000000000000000000000"

key length=0
string(128) "4fbf24aaa789f5194260ade1acd9499402c1845cc517e8fe43cfb5b90a0df294db33ecd1a836c47d6bf6d8600512ba415e17008a1e1991f81056258d82099397"
string(128) "546869732069732074686520736563726574206d657373616765207768696368206d75737420626520656e637279707465640000000000000000000000000000"

key length=8
string(128) "d6a3042b278fa5816dc6f46152acbe5fd7d1813c3808c27cd969d8e10a64d0238724edfda0322f4512308f22d142df0e92bed861c2b732f7650e234df59183dc"
string(128) "546869732069732074686520736563726574206d657373616765207768696368206d75737420626520656e637279707465640000000000000000000000000000"

key length=16
string(128) "dc8f957ec530acf10cd95ba7da7b6405380fe19a2941e9a8de54680512f18491bc374e5464885ae6c2ae2aa7a6cdd2fbe12a06bbc4bd59dbbfaa15f09044f101"
string(128) "546869732069732074686520736563726574206d657373616765207768696368206d75737420626520656e637279707465640000000000000000000000000000"

--- testing different iv lengths

iv length=0

Warning: mcrypt_cbc(): The IV parameter must be as long as the blocksize in %s on line %d
string(128) "c082b3fabaae4c8c410eb8dba64bae10e48d79b5241fb8f24462cad43bd0b35ad2746b00817e9dcbc636b44df0ec60b46a57e7a310a308a0947724e3817a13b4"

Warning: mcrypt_decrypt(): The IV parameter must be as long as the blocksize in %s on line %d
string(128) "546869732069732074686520736563726574206d657373616765207768696368206d75737420626520656e637279707465640000000000000000000000000000"

iv length=0

Warning: mcrypt_cbc(): The IV parameter must be as long as the blocksize in %s on line %d
string(128) "c082b3fabaae4c8c410eb8dba64bae10e48d79b5241fb8f24462cad43bd0b35ad2746b00817e9dcbc636b44df0ec60b46a57e7a310a308a0947724e3817a13b4"

Warning: mcrypt_decrypt(): The IV parameter must be as long as the blocksize in %s on line %d
string(128) "546869732069732074686520736563726574206d657373616765207768696368206d75737420626520656e637279707465640000000000000000000000000000"

iv length=8

Warning: mcrypt_cbc(): The IV parameter must be as long as the blocksize in %s on line %d
string(128) "c082b3fabaae4c8c410eb8dba64bae10e48d79b5241fb8f24462cad43bd0b35ad2746b00817e9dcbc636b44df0ec60b46a57e7a310a308a0947724e3817a13b4"

Warning: mcrypt_decrypt(): The IV parameter must be as long as the blocksize in %s on line %d
string(128) "546869732069732074686520736563726574206d657373616765207768696368206d75737420626520656e637279707465640000000000000000000000000000"

iv length=16
string(128) "dc8f957ec530acf10cd95ba7da7b6405380fe19a2941e9a8de54680512f18491bc374e5464885ae6c2ae2aa7a6cdd2fbe12a06bbc4bd59dbbfaa15f09044f101"
string(128) "546869732069732074686520736563726574206d657373616765207768696368206d75737420626520656e637279707465640000000000000000000000000000"

iv length=17

Warning: mcrypt_cbc(): The IV parameter must be as long as the blocksize in %s on line %d
string(128) "c082b3fabaae4c8c410eb8dba64bae10e48d79b5241fb8f24462cad43bd0b35ad2746b00817e9dcbc636b44df0ec60b46a57e7a310a308a0947724e3817a13b4"

Warning: mcrypt_decrypt(): The IV parameter must be as long as the blocksize in %s on line %d
string(128) "546869732069732074686520736563726574206d657373616765207768696368206d75737420626520656e637279707465640000000000000000000000000000"
===DONE===