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

echo "\n-- Testing hash_pbkdf2() function with less than expected no. of arguments --\n";
var_dump(hash_pbkdf2());
var_dump(hash_pbkdf2('md5'));
var_dump(hash_pbkdf2('md5', $password));
var_dump(hash_pbkdf2('md5', $password, $salt));

echo "\n-- Testing hash_pbkdf2() function with more than expected no. of arguments --\n";
var_dump(hash_pbkdf2('md5', $password, $salt, 10, 10, true, 'extra arg'));

echo "\n-- Testing hash_pbkdf2() function with invalid hash algorithm --\n";
var_dump(hash_pbkdf2('foo', $password, $salt, 1));

echo "\n-- Testing hash_pbkdf2() function with non-cryptographic hash algorithm --\n";
var_dump(hash_pbkdf2('crc32', $password, $salt, 1));

echo "\n-- Testing hash_pbkdf2() function with invalid iterations --\n";
var_dump(hash_pbkdf2('md5', $password, $salt, 0));
var_dump(hash_pbkdf2('md5', $password, $salt, -1));

echo "\n-- Testing hash_pbkdf2() function with invalid length --\n";
var_dump(hash_pbkdf2('md5', $password, $salt, 1, -1));

?>
===Done===
--EXPECTF--
*** Testing hash_pbkdf2() : error conditions ***

-- Testing hash_pbkdf2() function with less than expected no. of arguments --

Warning: hash_pbkdf2() expects at least 4 parameters, 0 given in %s on line %d
NULL

Warning: hash_pbkdf2() expects at least 4 parameters, 1 given in %s on line %d
NULL

Warning: hash_pbkdf2() expects at least 4 parameters, 2 given in %s on line %d
NULL

Warning: hash_pbkdf2() expects at least 4 parameters, 3 given in %s on line %d
NULL

-- Testing hash_pbkdf2() function with more than expected no. of arguments --

Warning: hash_pbkdf2() expects at most 6 parameters, 7 given in %s on line %d
NULL

-- Testing hash_pbkdf2() function with invalid hash algorithm --

Warning: hash_pbkdf2(): Unknown hashing algorithm: foo in %s on line %d
bool(false)

-- Testing hash_pbkdf2() function with non-cryptographic hash algorithm --

Warning: hash_pbkdf2(): Non-cryptographic hashing algorithm: crc32 in %s on line %d
bool(false)

-- Testing hash_pbkdf2() function with invalid iterations --

Warning: hash_pbkdf2(): Iterations must be a positive integer: 0 in %s on line %d
bool(false)

Warning: hash_pbkdf2(): Iterations must be a positive integer: -1 in %s on line %d
bool(false)

-- Testing hash_pbkdf2() function with invalid length --

Warning: hash_pbkdf2(): Length must be greater than or equal to 0: -1 in %s on line %d
bool(false)
===Done===
