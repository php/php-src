--TEST--
dns_get_record() basic usage with A record
--SKIPIF--
<?php require "skipif.inc"; ?>
--FILE--
<?php
$domain = 'www.basic.dnstest.php.net';

$result = dns_get_record($domain, DNS_A);
var_dump($result);
?>
--EXPECTF--
array(1) {
  [0]=>
  array(5) {
    ["host"]=>
    string(25) "www.basic.dnstest.php.net"
    ["class"]=>
    string(2) "IN"
    ["ttl"]=>
    int(%d)
    ["type"]=>
    string(1) "A"
    ["ip"]=>
    string(9) "192.0.2.1"
  }
}
