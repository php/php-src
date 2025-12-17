--TEST--
GH-19994: openssl_get_cipher_methods inconsistent with fetched methods
--EXTENSIONS--
openssl
--FILE--
<?php

$ciphers = openssl_get_cipher_methods();

foreach ($ciphers as $cipher) {
    if (openssl_cipher_iv_length($cipher) === false) {
        var_dump($cipher);
    }
}

?>
--EXPECT--
