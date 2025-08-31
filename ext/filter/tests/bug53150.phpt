--TEST--
Bug #53150 (FILTER_FLAG_NO_RES_RANGE is missing some IP ranges)
--EXTENSIONS--
filter
--FILE--
<?php
var_dump(filter_var('127.0.0.1', FILTER_VALIDATE_IP));
var_dump(filter_var("::1", FILTER_VALIDATE_IP));

var_dump(filter_var('127.0.0.1', FILTER_VALIDATE_IP, FILTER_FLAG_NO_RES_RANGE));
var_dump(filter_var('::1', FILTER_VALIDATE_IP, FILTER_FLAG_NO_RES_RANGE));

var_dump(filter_var('128.0.0.1', FILTER_VALIDATE_IP));
var_dump(filter_var('128.0.0.1', FILTER_VALIDATE_IP, FILTER_FLAG_NO_RES_RANGE));

var_dump(filter_var('191.255.0.0', FILTER_VALIDATE_IP));
var_dump(filter_var('191.255.0.0', FILTER_VALIDATE_IP, FILTER_FLAG_NO_RES_RANGE));

?>
--EXPECT--
string(9) "127.0.0.1"
string(3) "::1"
bool(false)
bool(false)
string(9) "128.0.0.1"
string(9) "128.0.0.1"
string(11) "191.255.0.0"
string(11) "191.255.0.0"
