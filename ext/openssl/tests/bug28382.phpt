--TEST--
Bug #28382 (openssl_x509_parse extensions support)
--EXTENSIONS--
openssl
--FILE--
<?php
$cert = file_get_contents(__DIR__ . "/bug28382cert.txt");
$ext = openssl_x509_parse($cert);
var_dump($ext['extensions']);
/*
 * The reason for %A at the end of crlDistributionPoints and authorityKeyIdentifier is that
 * OpenSSL 3.0 removes new lines which were present in previous versions.
 */
?>
--EXPECTF--
array(11) {
  ["basicConstraints"]=>
  string(8) "CA:FALSE"
  ["nsComment"]=>
  string(38) "For Grid use only; request tag userTag"
  ["nsCertType"]=>
  string(30) "SSL Client, SSL Server, S/MIME"
  ["crlDistributionPoints"]=>
  string(%d) "%AURI:http://mobile.blue-software.ro:90/ca/crl.shtml%A"
  ["nsCaPolicyUrl"]=>
  string(38) "http://mobile.blue-software.ro:90/pub/"
  ["subjectAltName"]=>
  string(28) "email:sergiu@bluesoftware.ro"
  ["subjectKeyIdentifier"]=>
  string(59) "B0:A7:FF:F9:41:15:DE:23:39:BD:DD:31:0F:97:A0:B2:A2:74:E0:FC"
  ["authorityKeyIdentifier"]=>
  string(%d) "DirName:/C=RO/ST=Romania/L=Craiova/O=Sergiu/OU=Sergiu SRL/CN=Sergiu CA/emailAddress=n_sergiu@hotmail.com
serial:00%A"
  ["keyUsage"]=>
  string(71) "Digital Signature, Non Repudiation, Key Encipherment, Data Encipherment"
  ["nsBaseUrl"]=>
  string(20) "http://62.231.98.52/"
  ["1.2.3.4"]=>
  string(4) "%s"
}
