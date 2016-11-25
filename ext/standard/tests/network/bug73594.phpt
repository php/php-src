--TEST--
Bug #73594 (dns_get_record() does not populate $additional out parameter)
--SKIPIF--
<?php
if (getenv("SKIP_ONLINE_TESTS")) die("skip test requiring internet connection");
?>
--FILE--
<?php
$auth = array();
$additional = array();
dns_get_record('php.net', DNS_ANY, $auth, $additional);
var_dump(empty($additional));
?>
--EXPECT--
bool(false)
