--TEST--
openssl_cipher_key_length() error test
--EXTENSIONS--
openssl
--FILE--
<?php

var_dump(openssl_cipher_key_length('unknown'));

try {
    var_dump(openssl_cipher_key_length(''));
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECTF--

Warning: openssl_cipher_key_length(): Unknown cipher algorithm in %s on line %d
bool(false)
openssl_cipher_key_length(): Argument #1 ($cipher_algo) cannot be empty
