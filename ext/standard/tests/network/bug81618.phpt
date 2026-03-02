--TEST--
Bug #81618: dns_get_record failure on FreeBSD
--SKIPIF--
<?php
if (getenv('SKIP_ONLINE_TESTS')) die('skip online test');
?>
--FILE--
<?php
$ret = dns_get_record('www.google.com', DNS_A + DNS_CNAME);

echo ($ret !== false && count($ret) > 0);

?>
--EXPECT--
1
