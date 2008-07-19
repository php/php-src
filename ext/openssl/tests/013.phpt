--TEST--
openssl_open() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$data = b"openssl_open() test";
$pub_key = "file://" . dirname(__FILE__) . "/public.key";
$priv_key = "file://" . dirname(__FILE__) . "/private.key";
$wrong = "wrong";
$wrong2 = b"wrong";

openssl_seal($data, $sealed, $ekeys, array($pub_key, $pub_key, $pub_key));
openssl_open($sealed, $output, $ekeys[0], $priv_key);
var_dump($output);
openssl_open($sealed, $output2, $ekeys[1], $wrong);
var_dump($output2);
openssl_open($sealed, $output3, $ekeys[2], $priv_key);
var_dump($output3);
openssl_open($sealed, $output4, $wrong, $priv_key);
var_dump($output4);
openssl_open($sealed, $output5, $wrong2, $priv_key);
var_dump($output5);
?>
--EXPECTF--
string(19) "openssl_open() test"

Warning: openssl_open(): Binary string expected, Unicode string received in %s on line %d

Warning: openssl_open(): unable to coerce parameter 4 into a private key in %s on line %d
NULL
string(19) "openssl_open() test"

Warning: openssl_open() expects parameter 3 to be strictly a binary string, Unicode string given in %s on line %d
NULL
NULL
