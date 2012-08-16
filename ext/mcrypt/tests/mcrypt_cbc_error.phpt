--TEST--
Test mcrypt_cbc() function : error conditions 
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

echo "*** Testing mcrypt_cbc() : error conditions ***\n";


//Test mcrypt_cbc with one more than the expected number of arguments
echo "\n-- Testing mcrypt_cbc() function with more than expected no. of arguments --\n";
$cipher = 10;
$key = 'string_val';
$data = 'string_val';
$mode = 10;
$iv = 'string_val';
$extra_arg = 10;
var_dump( mcrypt_cbc($cipher, $key, $data, $mode, $iv, $extra_arg) );

// Testing mcrypt_cbc with one less than the expected number of arguments
echo "\n-- Testing mcrypt_cbc() function with less than expected no. of arguments --\n";
$cipher = 10;
$key = 'string_val';
$data = 'string_val';
var_dump( mcrypt_cbc($cipher, $key, $data) );

?>
===DONE===
--EXPECTF--
*** Testing mcrypt_cbc() : error conditions ***

-- Testing mcrypt_cbc() function with more than expected no. of arguments --

Deprecated: Function mcrypt_cbc() is deprecated in %s on line %d

Warning: mcrypt_cbc() expects at most 5 parameters, 6 given in %s on line %d
NULL

-- Testing mcrypt_cbc() function with less than expected no. of arguments --

Deprecated: Function mcrypt_cbc() is deprecated in %s on line %d

Warning: mcrypt_cbc() expects at least 4 parameters, 3 given in %s on line %d
NULL
===DONE===

