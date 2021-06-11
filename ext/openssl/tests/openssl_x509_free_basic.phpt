--TEST--
openssl_x509_free() tests
--EXTENSIONS--
openssl
--FILE--
<?php
var_dump($res = openssl_x509_read("file://" . __DIR__ . "/cert.crt"));
openssl_x509_free($res);
var_dump($res);
?>
--EXPECTF--
object(OpenSSLCertificate)#1 (0) {
}

Deprecated: Function openssl_x509_free() is deprecated in %s on line %d
object(OpenSSLCertificate)#1 (0) {
}
