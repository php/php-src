--TEST--
openssl_x509_free() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
var_dump($res = openssl_x509_read("file://" . dirname(__FILE__) . "/cert.crt"));
openssl_x509_free($res);
var_dump($res);
openssl_x509_free(false);
?>
--EXPECTF--
resource(%d) of type (OpenSSL X.509)
resource(%d) of type (Unknown)

Warning: openssl_x509_free() expects parameter 1 to be resource, boolean given in %s on line %d
