--TEST--
openssl_open() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$data = "openssl_open() test";
$pub_key = "file://" . __DIR__ . "/public.key";
$priv_key = "file://" . __DIR__ . "/private_rsa_1024.key";
$wrong = "wrong";
$method = "RC4";

openssl_seal($data, $sealed, $ekeys, array($pub_key, $pub_key, $pub_key), $method);
openssl_open($sealed, $output, $ekeys[0], $priv_key, $method);
var_dump($output);
openssl_open($sealed, $output2, $ekeys[1], $wrong, $method);
var_dump($output2);
openssl_open($sealed, $output3, $ekeys[2], $priv_key, $method);
var_dump($output3);
openssl_open($sealed, $output4, $wrong, $priv_key, $method);
var_dump($output4);
?>
--EXPECTF--
string(19) "openssl_open() test"

Warning: openssl_open(): Unable to coerce parameter 4 into a private key in %s on line %d
NULL
string(19) "openssl_open() test"
NULL
