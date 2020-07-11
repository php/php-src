--TEST--
openssl_x509_free() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
var_dump($res = openssl_x509_read("file://" . __DIR__ . "/cert.crt"));
openssl_x509_free($res);
var_dump($res);
?>
--EXPECTF--
resource(%d) of type (OpenSSL X.509)
resource(%d) of type (Unknown)
