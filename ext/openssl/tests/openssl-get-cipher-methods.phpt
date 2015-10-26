--TEST--
array openssl_get_cipher_methods ([ bool $aliases = false ] );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--SKIPIF--
<?php 
if (!extension_loaded("openssl")) print "skip";
if (OPENSSL_VERSION_NUMBER < 0x10000000) die("skip Output requires OpenSSL 1.0");
?>
--FILE--
<?php
$getCipherTrue = openssl_get_cipher_methods(true);
$getCipherFalse = openssl_get_cipher_methods(false);

if ((is_array($getCipherTrue)) && (!is_null($getCipherTrue))) {
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
