--TEST--
Hash: hash_hmac_algos() function : basic functionality
--FILE--
<?php

print_r(hash_hmac_algos());
?>
--EXPECTF--
Array
(
    [%d] => md2
    [%d] => md4
    [%d] => md5
    [%d] => sha1
    [%d] => sha224
    [%d] => sha256
    [%d] => sha384
    [%d] => sha512/224
    [%d] => sha512/256
    [%d] => sha512
    [%d] => sha3-224
    [%d] => sha3-256
    [%d] => sha3-384
    [%d] => sha3-512
    [%d] => ripemd128
    [%d] => ripemd160
    [%d] => ripemd256
    [%d] => ripemd320
    [%d] => whirlpool
    [%d] => tiger128,3
    [%d] => tiger160,3
    [%d] => tiger192,3
    [%d] => tiger128,4
    [%d] => tiger160,4
    [%d] => tiger192,4
    [%d] => snefru
    [%d] => snefru256
    [%d] => gost
    [%d] => gost-crypto
    [%d] => haval128,3
    [%d] => haval160,3
    [%d] => haval192,3
    [%d] => haval224,3
    [%d] => haval256,3
    [%d] => haval128,4
    [%d] => haval160,4
    [%d] => haval192,4
    [%d] => haval224,4
    [%d] => haval256,4
    [%d] => haval128,5
    [%d] => haval160,5
    [%d] => haval192,5
    [%d] => haval224,5
    [%d] => haval256,5
)
