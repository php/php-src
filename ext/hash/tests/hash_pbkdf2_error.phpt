--TEST--
Hash: Test hash_pbkdf2() function : error functionality
--FILE--
<?php

    require_once __DIR__ . '/exceptional.inc';

/* {{{ proto string hash_pbkdf2(string algo, string password, string salt, int iterations [, int length = 0, bool raw_output = false])
Generate a PBKDF2 hash of the given password and salt
Returns lowercase hexbits by default */

echo "*** Testing hash_pbkdf2() : error conditions ***\n";

$password = 'password';
$salt = 'salt';

echo "\n-- Testing hash_pbkdf2() function with invalid hash algorithm --\n";
trycatch_dump(fn() => hash_pbkdf2('foo', $password, $salt, 1));

echo "\n-- Testing hash_pbkdf2() function with non-cryptographic hash algorithm --\n";
trycatch_dump(fn() => hash_pbkdf2('crc32', $password, $salt, 1));

echo "\n-- Testing hash_pbkdf2() function with invalid iterations --\n";
trycatch_dump(fn() => hash_pbkdf2('md5', $password, $salt, 0));
trycatch_dump(fn() => hash_pbkdf2('md5', $password, $salt, -1));

echo "\n-- Testing hash_pbkdf2() function with invalid length --\n";
trycatch_dump(fn() => hash_pbkdf2('md5', $password, $salt, 1, -1));

?>
===Done===
--EXPECT--
*** Testing hash_pbkdf2() : error conditions ***

-- Testing hash_pbkdf2() function with invalid hash algorithm --
[ErrorException] Parameter 1 is invalid: Unknown hashing algorithm: foo

-- Testing hash_pbkdf2() function with non-cryptographic hash algorithm --
[ErrorException] Parameter 1 is invalid: Non-cryptographic hashing algorithm: crc32

-- Testing hash_pbkdf2() function with invalid iterations --
[ErrorException] Parameter 4 is invalid: Iterations must be a positive integer: 0
[ErrorException] Parameter 4 is invalid: Iterations must be a positive integer: -1

-- Testing hash_pbkdf2() function with invalid length --
[ErrorException] Parameter 5 is invalid: Length must be greater than or equal to 0: -1
===Done===
