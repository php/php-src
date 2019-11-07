--TEST--
Hash: Test hash_pbkdf2() function : error functionality
--FILE--
<?php

/* {{{ proto string hash_pbkdf2(string algo, string password, string salt, int iterations [, int length = 0, bool raw_output = false])
Generate a PBKDF2 hash of the given password and salt
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
Unknown hashing algorithm: foo

-- Testing hash_pbkdf2() function with non-cryptographic hash algorithm --
Non-cryptographic hashing algorithm: crc32

-- Testing hash_pbkdf2() function with invalid iterations --
Iterations must be a positive integer: 0
Iterations must be a positive integer: -1

-- Testing hash_pbkdf2() function with invalid length --
Length must be greater than or equal to 0: -1
