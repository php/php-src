--TEST--
Request #70438: Add IV parameter for openssl_seal and openssl_open
--SKIPIF--
<?php
if (!extension_loaded("openssl")) print "skip";
if (!in_array('AES-128-CBC', openssl_get_cipher_methods(true))) {
    print "skip";
}
?>
--FILE--
<?php
$data = "openssl_seal() test";
$cipher = 'AES-128-CBC';
$pub_key = "file://" . dirname(__FILE__) . "/public.key";
$priv_key = "file://" . dirname(__FILE__) . "/private_rsa_1024.key";

openssl_seal($data, $sealed, $ekeys, array($pub_key, $pub_key), $cipher);
openssl_seal($data, $sealed, $ekeys, array($pub_key, $pub_key), 'sparkles', $iv);
openssl_seal($data, $sealed, $ekeys, array($pub_key, $pub_key), $cipher, $iv);
openssl_open($sealed, $decrypted, $ekeys[0], $priv_key, $cipher, $iv);
echo $decrypted;
?>
--EXPECTF--
Warning: openssl_seal(): Cipher algorithm requires an IV to be supplied as a sixth parameter in %s on line %d

Warning: openssl_seal(): Unknown signature algorithm. in %s on line %d
openssl_seal() test
