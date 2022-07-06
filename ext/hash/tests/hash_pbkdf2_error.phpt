--TEST--
Hash: Test hash_pbkdf2() function : error functionality
--FILE--
<?php

/* Generate a PBKDF2 hash of the given password and salt
Returns lowercase hexbits by default */

echo "*** Testing hash_pbkdf2() : error conditions ***\n";

$password = 'password';
$salt = 'salt';

echo "\n-- Testing hash_pbkdf2() function with invalid hash algorithm --\n";
try {
    var_dump(hash_pbkdf2('foo', $password, $salt, 1));
}
catch (\Error $e) {
    echo $e->getMessage() . "\n";
}


echo "\n-- Testing hash_pbkdf2() function with non-cryptographic hash algorithm --\n";
try {
    var_dump(hash_pbkdf2('crc32', $password, $salt, 1));
}
catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

echo "\n-- Testing hash_pbkdf2() function with invalid iterations --\n";
try {
    var_dump(hash_pbkdf2('md5', $password, $salt, 0));
}
catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump(hash_pbkdf2('md5', $password, $salt, -1));
}
catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

echo "\n-- Testing hash_pbkdf2() function with invalid length --\n";
try {
    var_dump(hash_pbkdf2('md5', $password, $salt, 1, -1));
}
catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
*** Testing hash_pbkdf2() : error conditions ***

-- Testing hash_pbkdf2() function with invalid hash algorithm --
hash_pbkdf2(): Argument #1 ($algo) must be a valid cryptographic hashing algorithm

-- Testing hash_pbkdf2() function with non-cryptographic hash algorithm --
hash_pbkdf2(): Argument #1 ($algo) must be a valid cryptographic hashing algorithm

-- Testing hash_pbkdf2() function with invalid iterations --
hash_pbkdf2(): Argument #4 ($iterations) must be greater than 0
hash_pbkdf2(): Argument #4 ($iterations) must be greater than 0

-- Testing hash_pbkdf2() function with invalid length --
hash_pbkdf2(): Argument #5 ($length) must be greater than or equal to 0
