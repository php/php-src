--TEST--
dns_get_record() with TXT record
--SKIPIF--
<?php require "skipif.inc"; ?>
--FILE--
<?php
$domain = 'txt1.basic.dnstest.php.net';

$result = dns_get_record($domain, DNS_TXT);
var_dump($result);
?>
--EXPECTF--
array(1) {
  [0]=>
  array(6) {
    ["host"]=>
    string(26) "txt1.basic.dnstest.php.net"
    ["class"]=>
    string(2) "IN"
    ["ttl"]=>
    int(%d)
    ["type"]=>
    string(3) "TXT"
    ["txt"]=>
    string(25) "This is a test TXT record"
    ["entries"]=>
    array(1) {
      [0]=>
      string(25) "This is a test TXT record"
    }
  }
}
