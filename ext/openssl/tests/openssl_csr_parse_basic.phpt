--TEST--
openssl_csr_parse() tests
--EXTENSIONS--
openssl
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
array(9) {
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
  ["attributes"]=>
  array(8) {
    ["streetAddress"]=>
    string(0) ""
    ["facsimileTelephoneNumber"]=>
    string(0) ""
    ["postalCode"]=>
    string(3) "N11"
    ["telephoneNumber"]=>
    string(9) "012345678"
    ["name"]=>
    string(12) "Organisation"
    ["1.3.6.1.4.1.11278.1150.2.1"]=>
    string(8) "11112222"
    ["1.3.6.1.4.1.11278.1150.2.2"]=>
    string(8) "12345678"
    ["emailAddress"]=>
    string(16) "info@example.com"
  }
  ["extensions"]=>
  array(1) {
    ["basicConstraints"]=>
    string(8) "CA:FALSE"
  }
}
array(9) {
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
  ["attributes"]=>
  array(8) {
    ["streetAddress"]=>
    string(0) ""
    ["facsimileTelephoneNumber"]=>
    string(0) ""
    ["postalCode"]=>
    string(3) "N11"
    ["telephoneNumber"]=>
    string(9) "012345678"
    ["name"]=>
    string(12) "Organisation"
    ["1.3.6.1.4.1.11278.1150.2.1"]=>
    string(8) "11112222"
    ["1.3.6.1.4.1.11278.1150.2.2"]=>
    string(8) "12345678"
    ["emailAddress"]=>
    string(16) "info@example.com"
  }
  ["extensions"]=>
  array(1) {
    ["basicConstraints"]=>
    string(8) "CA:FALSE"
  }
}
