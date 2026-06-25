--TEST--
OpenSSL password functions reject algorithm names containing NUL bytes
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists('openssl_password_hash')) {
    die('skip OpenSSL Argon2 support not available');
}
?>
--FILE--
<?php

foreach (["argon2i\0foo", "argon2id\0foo"] as $algo) {
    try {
        openssl_password_hash($algo, "secret");
    } catch (ValueError $e) {
        echo $e->getMessage(), PHP_EOL;
    }

    try {
        openssl_password_verify($algo, "secret", "digest");
    } catch (ValueError $e) {
        echo $e->getMessage(), PHP_EOL;
    }
}

?>
--EXPECT--
openssl_password_hash(): Argument #1 ($algo) must be a valid password openssl hashing algorithm
openssl_password_verify(): Argument #1 ($algo) must be a valid password openssl hashing algorithm
openssl_password_hash(): Argument #1 ($algo) must be a valid password openssl hashing algorithm
openssl_password_verify(): Argument #1 ($algo) must be a valid password openssl hashing algorithm
