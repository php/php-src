--TEST--
openssl_csr_parse() tests
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (OPENSSL_VERSION_NUMBER >= 0x30200000) die('skip For OpenSSL < 3.2');
?>
--FILE--
<?php
$csr = "file://" . __DIR__ . "/parse.csr";

$parsedCSR = openssl_csr_parse($csr);
var_dump($parsedCSR === openssl_csr_parse(file_get_contents($csr)));
var_dump($parsedCSR);
var_dump(openssl_csr_parse($csr, false));
?>
--EXPECTF--
bool(true)
array(8) {
  ["name"]=>
  string(71) "/C=UK/ST=England/L=London/CN=test.php.net/emailAddress=test.php@php.net"
  ["subject"]=>
  array(5) {
    ["C"]=>
    string(2) "UK"
    ["ST"]=>
    string(7) "England"
    ["L"]=>
    string(6) "London"
    ["CN"]=>
    string(12) "test.php.net"
    ["emailAddress"]=>
    string(16) "test.php@php.net"
  }
  ["hash"]=>
  string(8) "b21872c1"
  ["version"]=>
  int(0)
  ["signatureTypeSN"]=>
  string(10) "RSA-SHA256"
  ["signatureTypeLN"]=>
  string(23) "sha256WithRSAEncryption"
  ["signatureTypeNID"]=>
  int(668)
  ["extensions"]=>
  array(1) {
    ["basicConstraints"]=>
    string(8) "CA:FALSE"
  }
}
array(8) {
  ["name"]=>
  string(71) "/C=UK/ST=England/L=London/CN=test.php.net/emailAddress=test.php@php.net"
  ["subject"]=>
  array(5) {
    ["countryName"]=>
    string(2) "UK"
    ["stateOrProvinceName"]=>
    string(7) "England"
    ["localityName"]=>
    string(6) "London"
    ["commonName"]=>
    string(12) "test.php.net"
    ["emailAddress"]=>
    string(16) "test.php@php.net"
  }
  ["hash"]=>
  string(8) "b21872c1"
  ["version"]=>
  int(0)
  ["signatureTypeSN"]=>
  string(10) "RSA-SHA256"
  ["signatureTypeLN"]=>
  string(23) "sha256WithRSAEncryption"
  ["signatureTypeNID"]=>
  int(668)
  ["extensions"]=>
  array(1) {
    ["basicConstraints"]=>
    string(8) "CA:FALSE"
  }
}
