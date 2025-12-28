--TEST--
openssl_sign: invalid algorithm
--EXTENSIONS--
openssl
--FILE--
<?php
$dir = __DIR__;
$file_pub = $dir . '/bug37820cert.pem';
$file_key = $dir . '/bug37820key.pem';

$priv_key = file_get_contents($file_key);
$priv_key_id = openssl_get_privatekey($priv_key);

$data = "some custom data";
openssl_sign($data, $signature, $priv_key_id, "invalid algo");
?>
--EXPECTF--
Warning: openssl_sign(): Unknown digest algorithm in %s on line %d
