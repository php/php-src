--TEST--
Hash: hash_file() function : error conditions
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
$filename = 'hash_file_error_example.txt';
file_put_contents( $filename, 'The quick brown fox jumped over the lazy dog.' );


// hash_file() error tests
echo "\n-- Testing hash_file() function with an unknown algorithm --\n";
var_dump( hash_file( 'foobar', $filename ) );

echo "\n-- Testing hash_file() function with a non-existent file --\n";
var_dump( hash_file( 'md5', 'nonexistent.txt' ) );

?>
--CLEAN--
<?php

$filename = 'hash_file_error_example.txt';
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
