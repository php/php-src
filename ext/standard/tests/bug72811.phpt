--TEST--
Bug #72811 (parse_url fails with IPv6 only host)
--FILE--
<?php

var_dump(parse_url('[::1]', PHP_URL_HOST));
var_dump(parse_url('[::1]:80', PHP_URL_HOST));
var_dump(parse_url('http://[::1]:80', PHP_URL_HOST));

var_dump(parse_url('[a::1]', PHP_URL_HOST));
var_dump(parse_url('[a::1]:80', PHP_URL_HOST));
var_dump(parse_url('http://[a::1]:80', PHP_URL_HOST));

var_dump(parse_url('[1::1]', PHP_URL_HOST));
var_dump(parse_url('[1::1]:80', PHP_URL_HOST));
var_dump(parse_url('http://[1::1]:80', PHP_URL_HOST));
?>
--EXPECT--
string(5) "[::1]"
string(5) "[::1]"
string(5) "[::1]"
string(6) "[a::1]"
string(6) "[a::1]"
string(6) "[a::1]"
string(6) "[1::1]"
string(6) "[1::1]"
string(6) "[1::1]"
