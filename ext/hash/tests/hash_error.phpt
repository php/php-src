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

echo "\n-- Testing hash() function with invalid hash algorithm --\n";
try {
    hash('foo', '');
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
*** Testing hash() : error conditions ***

-- Testing hash() function with invalid hash algorithm --
hash(): Argument #1 ($algo) must be a valid hashing algorithm
