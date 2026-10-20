--TEST--
Hash: hash() function : error conditions
--FILE--
<?php

echo "*** Testing hash() : error conditions ***\n";

echo "\n-- Testing hash() function with invalid hash algorithm --\n";
try {
    hash('foo', '');
} catch (ValueError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

?>
--EXPECT--
*** Testing hash() : error conditions ***

-- Testing hash() function with invalid hash algorithm --
ValueError: hash(): Argument #1 ($algo) must be a valid hashing algorithm
