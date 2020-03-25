--TEST--
Hash: hash_hmac_file() function : basic functionality
--FILE--
<?php

/* Prototype  : string hash_hmac_file ( string algo, string filename, string key [, bool raw_output] )
 * Description: Generate a keyed hash value using the HMAC method and the contents of a given file
 * Source code: ext/hash/hash.c
 * Alias to functions:
*/

echo "*** Testing hash() : error conditions ***\n";

$file = __DIR__ . "hash_file.txt";
$key = 'secret';

echo "\n-- Testing hash_hmac_file() function with invalid hash algorithm --\n";
try {
    var_dump(hash_hmac_file('foo', $file, $key, TRUE));
}
catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

echo "\n-- Testing hash_hmac_file() function with non-cryptographic hash algorithm --\n";
try {
    var_dump(hash_hmac_file('crc32', $file, $key, TRUE));
}
catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

echo "\n-- Testing hash_hmac_file() function with bad path --\n";
try {
    var_dump(hash_hmac_file('md5', $file.chr(0).$file, $key, TRUE));
}
catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
*** Testing hash() : error conditions ***

-- Testing hash_hmac_file() function with invalid hash algorithm --
hash_hmac_file(): Argument #1 ($algo) must be a valid cryptographic hashing algorithm

-- Testing hash_hmac_file() function with non-cryptographic hash algorithm --
hash_hmac_file(): Argument #1 ($algo) must be a valid cryptographic hashing algorithm

-- Testing hash_hmac_file() function with bad path --
hash_hmac_file(): Argument #2 ($data) must be a valid path
