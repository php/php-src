--TEST--
openssl_x509_parse() subjectAlternativeName test
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (OPENSSL_VERSION_NUMBER >= 0x30200000) die('skip For OpenSSL < 3.2');
?>
--FILE--
<?php
$cert = file_get_contents(__DIR__ . "/subjectAlternativeName.crt");
$ext = openssl_x509_parse($cert);
var_dump($ext['subjectAlternativeName']);
--EXPECTF--
array(11) {
  [0]=>
  array(2) {
    ["type"]=>
    string(3) "DNS"
    ["value"]=>
    string(11) "example.com"
  }
  [1]=>
  array(2) {
    ["type"]=>
    string(3) "DNS"
    ["value"]=>
    string(15) "www.example.com"
  }
  [2]=>
  array(2) {
    ["type"]=>
    string(3) "DNS"
    ["value"]=>
    string(21) "subdomain.example.com"
  }
  [3]=>
  array(2) {
    ["type"]=>
    string(10) "IP Address"
    ["value"]=>
    string(11) "192.168.1.1"
  }
  [4]=>
  array(2) {
    ["type"]=>
    string(10) "IP Address"
    ["value"]=>
    string(20) "2607:f0d0:1002:51::4"
  }
  [5]=>
  array(2) {
    ["type"]=>
    string(5) "email"
    ["value"]=>
    string(17) "admin@example.com"
  }
  [6]=>
  array(2) {
    ["type"]=>
    string(7) "DirName"
    ["value"]=>
    array(4) {
      ["2.5.4.3"]=>
      string(8) "John Doe"
      ["2.5.4.11"]=>
      string(7) "Testing"
      ["2.5.4.10"]=>
      string(17) "Example Org, Inc."
      ["2.5.4.6"]=>
      string(2) "US"
    }
  }
  [7]=>
  array(2) {
    ["type"]=>
    string(9) "othername"
    ["value"]=>
    array(1) {
      ["1.2.840.113549.1.9.2"]=>
      string(20) "UID_unstructuredName"
    }
  }
  [8]=>
  array(2) {
    ["type"]=>
    string(9) "othername"
    ["value"]=>
    array(1) {
      ["1.2.840.113549.1.9.20"]=>
      string(16) "UID_friendlyName"
    }
  }
  [9]=>
  array(2) {
    ["type"]=>
    string(13) "Registered ID"
    ["value"]=>
    string(7) "1.2.3.4"
  }
  [10]=>
  array(2) {
    ["type"]=>
    string(3) "URI"
    ["value"]=>
    string(27) "http://example.com/resource"
  }
}
