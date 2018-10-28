--TEST--
Hash: hash_init() function - errors test
--FILE--
<?php
echo "*** Testing hash_init(): error conditions ***\n";

echo "-- Testing hash_init() function with no parameters --\n";
var_dump(hash_init());

echo "-- Testing hash_init() function with unknown algorithms --\n";
var_dump(hash_init('dummy'));

echo "-- Testing hash_init() function with HASH_HMAC and non-cryptographic algorithms --\n";
var_dump(hash_init('crc32', HASH_HMAC));

echo "-- Testing hash_init() function with HASH_HMAC and no key --\n";
var_dump(hash_init('md5', HASH_HMAC));
var_dump(hash_init('md5', HASH_HMAC, null));
?>
--EXPECTF--
*** Testing hash_init(): error conditions ***
-- Testing hash_init() function with no parameters --

Warning: hash_init() expects at least 1 parameter, 0 given in %s on line %d
NULL
-- Testing hash_init() function with unknown algorithms --

Warning: hash_init(): Unknown hashing algorithm: dummy in %s on line %d
bool(false)
-- Testing hash_init() function with HASH_HMAC and non-cryptographic algorithms --

Warning: hash_init(): HMAC requested with a non-cryptographic hashing algorithm: crc32 in %s on line %d
bool(false)
-- Testing hash_init() function with HASH_HMAC and no key --

Warning: hash_init(): HMAC requested without a key %s on line %d
bool(false)

Warning: hash_init(): HMAC requested without a key %s on line %d
bool(false)
