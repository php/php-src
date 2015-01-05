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
$r = openssl_x509_parse($cert,$use_short_names=true);
sort($r['subject']);
var_dump( $r['subject'] );
?>
--EXPECTF--
array(11) {
  [0]=>
  string(14) "1550 Bryant st"
  [1]=>
  string(5) "94103"
  [2]=>
  string(7) "4586007"
  [3]=>
  string(2) "CA"
  [4]=>
  string(26) "COMODO EV Multi-Domain SSL"
  [5]=>
  string(20) "Private Organization"
  [6]=>
  string(10) "Rdio, Inc."
  [7]=>
  string(13) "San Francisco"
  [8]=>
  string(2) "US"
  [9]=>
  array(2) {
    [0]=>
    string(2) "US"
    [1]=>
    string(8) "Delaware"
  }
  [10]=>
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
}
