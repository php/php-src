--TEST--
Bug #61700 (FILTER_FLAG_IPV6/FILTER_FLAG_NO_PRIV|RES_RANGE failing)
--EXTENSIONS--
filter
--FILE--
<?php
var_dump(filter_var('::ffff:192.168.1.1', FILTER_VALIDATE_IP, FILTER_FLAG_IPV4));
var_dump(filter_var('::ffff:192.168.1.1', FILTER_VALIDATE_IP, FILTER_FLAG_NO_PRIV_RANGE));
var_dump(filter_var('0:0:0:0:0:0:0:1', FILTER_VALIDATE_IP, FILTER_FLAG_NO_RES_RANGE));
?>
--EXPECT--
bool(false)
string(18) "::ffff:192.168.1.1"
bool(false)
