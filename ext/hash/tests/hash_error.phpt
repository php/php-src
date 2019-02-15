--TEST--
Hash: hash() function : error conditions
--FILE--
<?php

/* Prototype  : string hash  ( string $algo  , string $data  [, bool $raw_output  ] )
 * Description: Generate a hash value (message digest)
 * Source code: ext/hash/hash.c
 * Alias to functions:
*/
echo "*** Testing hash() : error conditions ***\n";

echo "\n-- Testing hash() function with less than expected no. of arguments --\n";
var_dump(hash());
var_dump(hash('adler32'));

echo "\n-- Testing hash() function with more than expected no. of arguments --\n";
$extra_arg= 10;
var_dump(hash('adler32', '', false, $extra_arg));

echo "\n-- Testing hash() function with invalid hash algorithm --\n";
var_dump(hash('foo', ''));

?>
===Done===
--EXPECTF--
*** Testing hash() : error conditions ***

-- Testing hash() function with less than expected no. of arguments --

Warning: hash() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: hash() expects at least 2 parameters, 1 given in %s on line %d
NULL

-- Testing hash() function with more than expected no. of arguments --

Warning: hash() expects at most 3 parameters, 4 given in %s on line %d
NULL

-- Testing hash() function with invalid hash algorithm --

Warning: hash(): Unknown hashing algorithm: foo in %s on line %d
bool(false)
===Done===
