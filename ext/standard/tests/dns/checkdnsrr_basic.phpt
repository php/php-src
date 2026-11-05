--TEST--
checkdnsrr() and dns_check_record() basic usage
--SKIPIF--
<?php require "skipif.inc"; ?>
--FILE--
<?php
var_dump(checkdnsrr('www.basic.dnstest.php.net', 'A'));
var_dump(checkdnsrr('basic.dnstest.php.net', 'MX'));
var_dump(checkdnsrr('basic.dnstest.php.net', 'NS'));
var_dump(checkdnsrr('txt1.basic.dnstest.php.net', 'TXT'));
var_dump(dns_check_record('www.basic.dnstest.php.net', 'A'));
var_dump(checkdnsrr('www.basic.dnstest.php.net', 'MX'));
var_dump(checkdnsrr('nonexistent.basic.dnstest.php.net', 'A'));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
