--TEST--
Bug #64802: openssl_x509_parse fails to parse subject properly in some cases
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip");
if (!defined('OPENSSL_KEYTYPE_EC')) die("skip no EC available");
?>
--FILE--
<?php
$cert = file_get_contents(__DIR__.'/bug64802.pem');
$r = openssl_x509_parse($cert,$use_short_names=false);
var_dump($r['subject']['commonName']);
?>
--EXPECTF--
array(6) {
  [0]=>
  string(9) "www.rd.io"
  [1]=>
  string(8) "rdio.com"
  [2]=>
  string(5) "rd.io"
  [3]=>
  string(12) "api.rdio.com"
  [4]=>
  string(9) "api.rd.io"
  [5]=>
  string(12) "www.rdio.com"
}
