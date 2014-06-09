--TEST--
Bug #67403: Add signatureType to openssl_x509_parse
--SKIPIF--
<?php 
if (!extension_loaded("openssl")) die("skip");
--FILE--
<?php
$r = openssl_x509_parse(file_get_contents(__DIR__.'/bug64802.pem'));
var_dump($r['signatureType']);
$r = openssl_x509_parse(file_get_contents(__DIR__.'/bug37820cert.pem'));
var_dump($r['signatureType']);
--EXPECTF--
string(21) "sha1WithRSAEncryption"
string(20) "md5WithRSAEncryption"
