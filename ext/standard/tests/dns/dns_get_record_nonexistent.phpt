--TEST--
dns_get_record() with a name that does not exist
--SKIPIF--
<?php require "skipif.inc"; ?>
--FILE--
<?php
$domain = 'nonexistent.basic.dnstest.php.net';

var_dump(dns_get_record($domain, DNS_A));
var_dump(dns_get_record($domain, DNS_A, $authns, $addtl, true));
?>
--EXPECT--
array(0) {
}
array(0) {
}
