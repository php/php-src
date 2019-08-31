--TEST--
Hash: hash_init() function - errors test
--FILE--
<?php
echo "*** Testing hash_init(): error conditions ***\n";

echo "\n-- Testing hash_init() function with unknown algorithms --\n";
try {
    var_dump(hash_init('dummy'));
}
catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

echo "\n-- Testing hash_init() function with HASH_HMAC and non-cryptographic algorithms --\n";
try {
    var_dump(hash_init('crc32', HASH_HMAC));
}
catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

echo "\n-- Testing hash_init() function with HASH_HMAC and no key --\n";
try { 
    var_dump(hash_init('md5', HASH_HMAC));
}
catch (\Error $e) {
    echo  $e->getMessage() . "\n";
}

try { 
    var_dump(hash_init('md5', HASH_HMAC, null));
}
catch (\Error $e) {
    echo $e->getMessage() . "\n";
}


?>
--EXPECT--
*** Testing hash_init(): error conditions ***

-- Testing hash_init() function with unknown algorithms --
Unknown hashing algorithm: dummy

-- Testing hash_init() function with HASH_HMAC and non-cryptographic algorithms --
HMAC requested with a non-cryptographic hashing algorithm: crc32

-- Testing hash_init() function with HASH_HMAC and no key --
HMAC requested without a key
HMAC requested without a key
