--TEST--
#28382, openssl_x509_parse extensions support
--SKIPIF--
<?php 
if (!extension_loaded("openssl")) die("skip"); 
?>
--FILE--
<?php
$cert = file_get_contents(dirname(__FILE__) . "/bug28382cert.txt", "rb");
$ext = openssl_x509_parse($cert);
var_dump($ext['extensions']);
?>
--EXPECTF--
array(11) {
  ["basicConstraints"]=>
  string(2) "%s"
  ["nsComment"]=>
  string(40) "%s"
  ["nsCertType"]=>
  string(4) "%s"
  ["crlDistributionPoints"]=>
  string(56) "%s"
  ["nsCaPolicyUrl"]=>
  string(40) "%s"
  ["subjectAltName"]=>
  string(26) "%s"
  ["subjectKeyIdentifier"]=>
  string(22) "%s"
  ["authorityKeyIdentifier"]=>
  string(159) "%s"
  ["keyUsage"]=>
  string(4) "%s"
  ["nsBaseUrl"]=>
  string(22) "%s"
  ["UNDEF"]=>
  string(4) "%s"
}
