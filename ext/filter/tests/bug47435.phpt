--TEST--
Bug #47435 (FILTER_FLAG_NO_PRIV_RANGE and FILTER_FLAG_NO_RES_RANGE don't work with ipv6)
--EXTENSIONS--
filter
--FILE--
<?php
var_dump(filter_var("FC00::1", FILTER_VALIDATE_IP, FILTER_FLAG_IPV6));
var_dump(filter_var("FC00::1", FILTER_VALIDATE_IP, FILTER_FLAG_IPV6 | FILTER_FLAG_NO_PRIV_RANGE));
var_dump(filter_var("::", FILTER_VALIDATE_IP, FILTER_FLAG_IPV6));
var_dump(filter_var("::", FILTER_VALIDATE_IP, FILTER_FLAG_IPV6 | FILTER_FLAG_NO_RES_RANGE));
var_dump(filter_var("::1", FILTER_VALIDATE_IP, FILTER_FLAG_IPV6));
var_dump(filter_var("::1", FILTER_VALIDATE_IP, FILTER_FLAG_IPV6 | FILTER_FLAG_NO_RES_RANGE));
var_dump(filter_var("fe80:5:6::1", FILTER_VALIDATE_IP, FILTER_FLAG_IPV6));
var_dump(filter_var("fe80:5:6::1", FILTER_VALIDATE_IP, FILTER_FLAG_IPV6 | FILTER_FLAG_NO_RES_RANGE));
var_dump(filter_var("2001:0db8::1", FILTER_VALIDATE_IP, FILTER_FLAG_IPV6));
var_dump(filter_var("2001:0db8::1", FILTER_VALIDATE_IP, FILTER_FLAG_IPV6 | FILTER_FLAG_NO_RES_RANGE));
var_dump(filter_var("2001:0010::1", FILTER_VALIDATE_IP, FILTER_FLAG_IPV6));
var_dump(filter_var("2001:0010::1", FILTER_VALIDATE_IP, FILTER_FLAG_IPV6 | FILTER_FLAG_NO_RES_RANGE));
var_dump(filter_var("240b:0010::1", FILTER_VALIDATE_IP, FILTER_FLAG_IPV6));
var_dump(filter_var("240b:0010::1", FILTER_VALIDATE_IP, FILTER_FLAG_IPV6 | FILTER_FLAG_NO_RES_RANGE));
var_dump(filter_var("5f::1", FILTER_VALIDATE_IP, FILTER_FLAG_IPV6));
var_dump(filter_var("5f::1", FILTER_VALIDATE_IP, FILTER_FLAG_IPV6 | FILTER_FLAG_NO_RES_RANGE));
var_dump(filter_var("3ff3::1", FILTER_VALIDATE_IP, FILTER_FLAG_IPV6));
var_dump(filter_var("3ff3::1", FILTER_VALIDATE_IP, FILTER_FLAG_IPV6 | FILTER_FLAG_NO_RES_RANGE));
?>
--EXPECT--
string(7) "FC00::1"
bool(false)
string(2) "::"
bool(false)
string(3) "::1"
bool(false)
string(11) "fe80:5:6::1"
bool(false)
string(12) "2001:0db8::1"
bool(false)
string(12) "2001:0010::1"
bool(false)
string(12) "240b:0010::1"
string(12) "240b:0010::1"
string(5) "5f::1"
bool(false)
string(7) "3ff3::1"
bool(false)
