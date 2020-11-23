--TEST--
Hash: hash_file() function : error conditions
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>
--FILE--
<?php
echo "*** Testing hash_file() : error conditions ***\n";

// Set up file
$filename = 'hash_file_error_example.txt';
file_put_contents( $filename, 'The quick brown fox jumped over the lazy dog.' );


// hash_file() error tests
echo "\n-- Testing hash_file() function with an unknown algorithm --\n";
try {
    hash_file('foobar', $filename);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

echo "\n-- Testing hash_file() function with a non-existent file --\n";
var_dump(hash_file('md5', 'nonexistent.txt'));

?>
--CLEAN--
<?php

$filename = 'hash_file_error_example.txt';
unlink( $filename );

?>
--EXPECTF--
*** Testing hash_file() : error conditions ***

-- Testing hash_file() function with an unknown algorithm --
hash_file(): Argument #1 ($algo) must be a valid hashing algorithm

-- Testing hash_file() function with a non-existent file --

Warning: hash_file(%s): Failed to open stream: No such file or directory in %s on line %d
bool(false)
