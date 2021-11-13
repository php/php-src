--TEST--
Bug #81618: dns_get_record failure on FreeBSD
--FILE--
<?php
$ret = dns_get_record('www.google.com', DNS_A + DNS_CNAME);

echo ($ret !== false && count($ret) > 0);

?>

--EXPECT--
1
