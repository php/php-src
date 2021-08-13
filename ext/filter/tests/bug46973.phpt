--TEST--
Bug #46973 (IPv6 address filter rejects valid address)
--EXTENSIONS--
filter
--FILE--
<?php

var_dump(filter_var('1fff::a88:85a3::172.31.128.1', FILTER_VALIDATE_IP,FILTER_FLAG_IPV6));
var_dump(filter_var('3ffe:6a88:85a3:08d3:1319:8a2e:0370:7344', FILTER_VALIDATE_IP,FILTER_FLAG_IPV6));
var_dump(filter_var('1fff::a88:85a3::172.31.128.1', FILTER_VALIDATE_IP,FILTER_FLAG_IPV6));

?>
--EXPECT--
bool(false)
string(39) "3ffe:6a88:85a3:08d3:1319:8a2e:0370:7344"
bool(false)
