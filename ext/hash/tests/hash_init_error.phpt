--TEST--
Hash: hash_init() function - errors test
--FILE--
<?php

require_once __DIR__ . '/exceptional.inc';

echo "*** Testing hash_init(): error conditions ***\n";

echo "-- Testing hash_init() function with unknown algorithms --\n";
trycatch_dump(fn() => hash_init('dummy'));

echo "-- Testing hash_init() function with HASH_HMAC and non-cryptographic algorithms --\n";
trycatch_dump(fn() => hash_init('crc32', HASH_HMAC));

echo "-- Testing hash_init() function with HASH_HMAC and no key --\n";
trycatch_dump(fn() => hash_init('md5', HASH_HMAC));
trycatch_dump(fn() => hash_init('md5', HASH_HMAC, null));

?>
--EXPECT--
*** Testing hash_init(): error conditions ***
-- Testing hash_init() function with unknown algorithms --
[ErrorException] Parameter 1 is invalid: Unknown hashing algorithm: dummy
-- Testing hash_init() function with HASH_HMAC and non-cryptographic algorithms --
[ErrorException] Parameter 1 is invalid: HMAC requested with a non-cryptographic hashing algorithm: crc32
-- Testing hash_init() function with HASH_HMAC and no key --
[ErrorException] Parameter 3 is invalid: HMAC requested without a key
[ErrorException] Parameter 3 is invalid: HMAC requested without a key
