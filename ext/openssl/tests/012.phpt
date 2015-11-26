--TEST--
openssl_seal() error tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$data = "openssl_open() test";
$pub_key = "file://" . dirname(__FILE__) . "/public.key";
$wrong = "wrong";

openssl_seal($data, $sealed, $ekeys, array($pub_key));                  // no output
openssl_seal($data, $sealed, $ekeys, array($pub_key, $pub_key));        // no output
openssl_seal($data, $sealed, $ekeys, array($pub_key, $wrong));
openssl_seal($data, $sealed, $ekeys, $pub_key);
openssl_seal($data, $sealed, $ekeys, array());
openssl_seal($data, $sealed, $ekeys, array($wrong));
?>
--EXPECTF--

Warning: openssl_seal(): not a public key (2th member of pubkeys) in %s on line %d

Warning: openssl_seal() expects parameter 4 to be array, string given in %s on line %d

Warning: openssl_seal(): Fourth argument to openssl_seal() must be a non-empty array in %s on line %d

Warning: openssl_seal(): not a public key (1th member of pubkeys) in %s on line %d

