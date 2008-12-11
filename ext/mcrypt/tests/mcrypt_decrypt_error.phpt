--TEST--
Test mcrypt_decrypt() function : error conditions 
--SKIPIF--
<?php 
if (!extension_loaded("mcrypt")) {
	print "skip - mcrypt extension not loaded"; 
}	 
?>
--FILE--
<?php
/* Prototype  : string mcrypt_decrypt(string cipher, string key, string data, string mode, string iv)
 * Description: OFB crypt/decrypt data using key key with cipher cipher starting with iv 
 * Source code: ext/mcrypt/mcrypt.c
 * Alias to functions: 
 */

echo "*** Testing mcrypt_decrypt() : error conditions ***\n";


//Test mcrypt_decrypt with one more than the expected number of arguments
echo "\n-- Testing mcrypt_decrypt() function with more than expected no. of arguments --\n";
$cipher = 'string_val';
$key = 'string_val';
$data = 'string_val';
$mode = 'string_val';
$iv = 'string_val';
$extra_arg = 10;
var_dump( mcrypt_decrypt($cipher, $key, $data, $mode, $iv, $extra_arg) );

// Testing mcrypt_decrypt with one less than the expected number of arguments
echo "\n-- Testing mcrypt_decrypt() function with less than expected no. of arguments --\n";
$cipher = 'string_val';
$key = 'string_val';
$data = 'string_val';
var_dump( mcrypt_decrypt($cipher, $key, $data) );

?>
===DONE===
--EXPECTF--
*** Testing mcrypt_decrypt() : error conditions ***

-- Testing mcrypt_decrypt() function with more than expected no. of arguments --

Warning: mcrypt_decrypt() expects at most 5 parameters, 6 given in %s on line %d
NULL

-- Testing mcrypt_decrypt() function with less than expected no. of arguments --

Warning: mcrypt_decrypt() expects at least 4 parameters, 3 given in %s on line %d
NULL
===DONE===

