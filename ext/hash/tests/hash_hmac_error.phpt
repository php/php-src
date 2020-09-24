--TEST--
Hash: hash_hmac() function : basic functionality
--FILE--
<?php
/*
* Function is implemented in ext/hash/hash.c
*/

echo "*** Testing hash_hmac() : error conditions ***\n";

$data = "This is a sample string used to test the hash_hmac function with various hashing algorithms";
$key = 'secret';

echo "\n-- Testing hash_hmac() function with invalid hash algorithm --\n";
try {
    var_dump(hash_hmac('foo', $data, $key));
}
catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

echo "\n-- Testing hash_hmac() function with non-cryptographic hash algorithm --\n";
try {
    var_dump(hash_hmac('crc32', $data, $key));
}
catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
*** Testing hash_hmac() : error conditions ***

-- Testing hash_hmac() function with invalid hash algorithm --
hash_hmac(): Argument #1 ($algo) must be a valid cryptographic hashing algorithm

-- Testing hash_hmac() function with non-cryptographic hash algorithm --
hash_hmac(): Argument #1 ($algo) must be a valid cryptographic hashing algorithm
