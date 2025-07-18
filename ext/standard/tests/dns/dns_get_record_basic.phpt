--TEST--
dns_get_record() basic usage
--SKIPIF--
<?php require "skipif.inc"; ?>
--FILE--
<?php
$domain = 'www.basic.dnstest.php.net';

$result = dns_get_record($domain, DNS_A);
var_dump($result);
?>
--EXPECTF--
WRONG
array(%d) {
  [0]=>
  array(%d) {
    ["host"]=>
    string(%d) "www.basic.dnstest.php.net"
    ["class"]=>
    string(2) "IN"
    ["ttl"]=>
    int(%d)
    ["type"]=>
    string(1) "A"
    ["ip"]=>
    string(%d) "192.0.2.1"
  }
}
