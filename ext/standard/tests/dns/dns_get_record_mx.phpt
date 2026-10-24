--TEST--
dns_get_record() with MX record
--SKIPIF--
<?php require "skipif.inc"; ?>
--FILE--
<?php
$domain = 'basic.dnstest.php.net';

$result = dns_get_record($domain, DNS_MX);
var_dump($result);
?>
--EXPECTF--
array(1) {
  [0]=>
  array(6) {
    ["host"]=>
    string(21) "basic.dnstest.php.net"
    ["class"]=>
    string(2) "IN"
    ["ttl"]=>
    int(%d)
    ["type"]=>
    string(2) "MX"
    ["pri"]=>
    int(10)
    ["target"]=>
    string(25) "mx1.basic.dnstest.php.net"
  }
}
