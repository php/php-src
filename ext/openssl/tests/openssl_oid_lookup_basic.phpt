--TEST--
openssl_csr_new() attributes setting tests
--EXTENSIONS--
openssl
--FILE--
<?php
var_dump(openssl_oid_lookup("CN"));
var_dump(openssl_oid_lookup("unstructuredAddress"));
var_dump(openssl_oid_lookup("1.2.3.4.5"));
var_dump(openssl_oid_lookup("junk"));
?>
--EXPECTF--
array(3) {
  ["oid"]=>
  string(7) "2.5.4.3"
  ["lname"]=>
  string(10) "commonName"
  ["sname"]=>
  string(2) "CN"
}
array(2) {
  ["oid"]=>
  string(20) "1.2.840.113549.1.9.8"
  ["lname"]=>
  string(19) "unstructuredAddress"
}
array(1) {
  ["oid"]=>
  string(9) "1.2.3.4.5"
}
bool(false)
