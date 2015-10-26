--TEST--
array openssl_get_cipher_methods ([ bool $aliases = false ] );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--SKIPIF--
<?php 
if (!extension_loaded("openssl")) print "skip";
?>
--FILE--
<?php
$get_cipher_true = openssl_get_cipher_methods(true);
$getCipherFalse = openssl_get_cipher_methods(false);

if ((is_array($get_cipher_true)) && (!is_null($get_cipher_true))) {
    print("okey");
} else {
    print("OpenSSL has failed to get cipher methods with the aliases argument set to true");
}

if ((is_array($getCipherFalse)) && (!is_null($getCipherFalse))) {
    print("okey");
} else {
    print("OpenSSL has failed to get cipher methods with the aliases argument set to false");
}
?>
--EXPECT--
okey
okey
