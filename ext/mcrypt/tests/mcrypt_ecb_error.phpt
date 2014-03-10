--TEST--
Test mcrypt_ecb() function : error conditions 
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

echo "*** Testing mcrypt_ecb() : error conditions ***\n";


//Test mcrypt_ecb with one more than the expected number of arguments
echo "\n-- Testing mcrypt_ecb() function with more than expected no. of arguments --\n";
$cipher = 10;
$key = 'string_val';
$data = 'string_val';
$mode = 10;
$iv = 'string_val';
$extra_arg = 10;
var_dump( mcrypt_ecb($cipher, $key, $data, $mode, $iv, $extra_arg) );

// Testing mcrypt_ecb with one less than the expected number of arguments
echo "\n-- Testing mcrypt_ecb() function with less than expected no. of arguments --\n";
$cipher = 10;
$key = 'string_val';
$data = 'string_val';
var_dump( mcrypt_ecb($cipher, $key, $data) );

?>
===DONE===
--EXPECTF--
*** Testing mcrypt_ecb() : error conditions ***

-- Testing mcrypt_ecb() function with more than expected no. of arguments --

Warning: mcrypt_ecb() expects at most 5 parameters, 6 given in %s on line %d
NULL

-- Testing mcrypt_ecb() function with less than expected no. of arguments --

Warning: mcrypt_ecb() expects at least 4 parameters, 3 given in %s on line %d
NULL
===DONE===

