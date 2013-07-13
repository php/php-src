--TEST--
Test hash_file() function : error conditions 
--SKIPIF--
<?php extension_loaded('hash') or die('skip: hash extension not loaded.'); ?>
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>
--FILE--
<?php
/* Prototype  : string hash_file(string algo, string filename[, bool raw_output = false])
 * Description: Generate a hash of a given file
 * Source code: ext/hash/hash.c
 * Alias to functions: 
 */

echo "*** Testing hash_file() : error conditions ***\n";

// Set up file
$filename = 'hash_file_example.txt';
file_put_contents( $filename, 'The quick brown fox jumped over the lazy dog.' );


// hash_file() error tests
echo "\n-- Testing hash_file() function with an unknown algorithm --\n";
var_dump( hash_file( 'foobar', $filename ) );

echo "\n-- Testing hash_file() function with a non-existent file --\n";
var_dump( hash_file( 'md5', 'nonexistent.txt' ) );

echo "\n-- Testing hash_file() function with less than expected no. of arguments --\n";
var_dump( hash_file( 'md5' ) );

echo "\n-- Testing hash_file() function with more than expected no. of arguments --\n";
$extra_arg = 10;
var_dump( hash_file( 'md5', $filename, false, $extra_arg ) );

?>
===DONE===
--CLEAN--
<?php

$filename = 'hash_file_example.txt';
unlink( $filename );

?>
--EXPECTF--
*** Testing hash_file() : error conditions ***

-- Testing hash_file() function with an unknown algorithm --

Warning: hash_file(): Unknown hashing algorithm: %s in %s on line %d
bool(false)

-- Testing hash_file() function with a non-existent file --

Warning: hash_file(%s): failed to open stream: No such file or directory in %s on line %d
bool(false)

-- Testing hash_file() function with less than expected no. of arguments --

Warning: hash_file() expects at least 2 parameters, 1 given in %s on line %d
NULL

-- Testing hash_file() function with more than expected no. of arguments --

Warning: hash_file() expects at most 3 parameters, 4 given in %s on line %d
NULL
===DONE===
