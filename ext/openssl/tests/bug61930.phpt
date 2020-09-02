--TEST--
Bug #61930: openssl corrupts ssl key resource when using openssl_get_publickey()
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip");
?>
--FILE--
<?php
$cert = file_get_contents(__DIR__.'/cert.crt');

$data = <<<DATA
Please verify me
DATA;

$sig = 'f9Gyb6NV/ENn7GUa37ygTLcF93XHf5fbFTnoYF/O+fXbq3iChGUbET0RuhOsptl' .
        'AODi6JsDLnJO4ikcVZo0tC1fFTj3LyCuPy3ZdgJbbVxQ/rviROCmuMFTqUW/Xa2' .
        'LQYiapeCCgLQeWTLg7TM/BoHEkKbKLG/XT5jHvep1758A=';

$key = openssl_get_publickey($cert);
var_dump(openssl_get_publickey($key));
var_dump(openssl_verify($data, base64_decode($sig), $key));
?>
--EXPECTF--
object(OpenSSLAsymmetricKey)#%d (0) {
}
int(1)
