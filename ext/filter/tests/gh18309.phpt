--TEST--
GH-18309 (ipv6 filter integer overflow)
--EXTENSIONS--
filter
--FILE--
<?php
var_dump(filter_var('fffffffffffffffffffffffffffffffffffff::', FILTER_VALIDATE_IP, FILTER_FLAG_IPV6));
?>
--EXPECT--
bool(false)
