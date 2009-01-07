--TEST--
Test hash_file() function : error conditions
--SKIPIF--
<?php 
	if(!extension_loaded("hash")) print "skip"; 
?>
--FILE--
<?php

/* Prototype  : string hash_file ( string algo, string filename [, bool raw_output] )
 * Description: Generate a hash value using the contents of a given file
 * Source code: ext/hash/hash.c
 * Alias to functions: 
*/

echo "*** Testing hash_file() : error conditions ***\n";

$file = dirname(__FILE__) . "hash_file.txt";

echo "\n-- Testing hash_file() function with less than expected no. of arguments --\n";
var_dump(hash_file());
var_dump(hash_file('crc32'));

echo "\n-- Testing hash_file() function with more than expected no. of arguments --\n";
var_dump(hash_file('crc32', $file, TRUE, 10));

echo "\n-- Testing hash_file() function with invalid hash algorithm --\n";
var_dump(hash_file('foo', $file));

?>
===Done===
--EXPECTF--
*** Testing hash_file() : error conditions ***

-- Testing hash_file() function with less than expected no. of arguments --

Warning: hash_file() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: hash_file() expects at least 2 parameters, 1 given in %s on line %d
NULL

-- Testing hash_file() function with more than expected no. of arguments --

Warning: hash_file() expects at most 3 parameters, 4 given in %s on line %d
NULL

-- Testing hash_file() function with invalid hash algorithm --

Warning: hash_file(): Unknown hashing algorithm: foo in %s on line %d
bool(false)
===Done===