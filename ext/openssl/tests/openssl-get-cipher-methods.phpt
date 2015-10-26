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
$get_cipher_false = openssl_get_cipher_methods(false);

if (is_array($get_cipher_true)) {
    print("okey");
} else {
    print("OpenSSL has failed to get cipher methods with the aliases argument set to true");
}

if ((is_array($get_cipher_false)) && (!is_null($get_cipher_false))) {
    print("okey");
} else {
    print("OpenSSL has failed to get cipher methods with the aliases argument set to false");
}
?>
--EXPECT--
okey
okey
