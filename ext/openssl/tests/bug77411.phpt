--TEST--
Bug #77411 (Large serial number returned as hexadecimal with OpenSSL version 1.1)
--SKIPIF--
<?php
if (!extension_loaded('openssl')) die('skip');
?>
--FILE--
<?php
$parsedCert = openssl_x509_parse('file://' . __DIR__ . '/bug77411cert.txt');
print_r($parsedCert['serialNumber']);
echo "\n";
print_r($parsedCert['serialNumberHex']);
echo "\n";
?>
--EXPECT--
723576652291400353363935363462355895043073364003
7EBE4C9BE37BE5CBFAA3345939DB1F88F60DD023
