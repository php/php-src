--TEST--
Bug #72811 (parse_url fails with IPv6 only host)
--FILE--
<?php

var_dump(parse_url('[::1]/foo.php', PHP_URL_HOST));
var_dump(parse_url('[::1]/foo.php', PHP_URL_PATH));
var_dump(parse_url('[::1]:80/foo.php', PHP_URL_HOST));
var_dump(parse_url('[::1]:80/foo.php', PHP_URL_PORT));
var_dump(parse_url('[::1]:80/foo.php', PHP_URL_PATH));
var_dump(parse_url('http://[::1]:80/foo.php', PHP_URL_SCHEME));
var_dump(parse_url('http://[::1]:80/foo.php', PHP_URL_HOST));
var_dump(parse_url('http://[::1]:80/foo.php', PHP_URL_PORT));
var_dump(parse_url('http://[::1]:80/foo.php', PHP_URL_PATH));

var_dump(parse_url('[1::1]/foo.php', PHP_URL_HOST));
var_dump(parse_url('[1::1]/foo.php', PHP_URL_PATH));
var_dump(parse_url('[1::1]:80/foo.php', PHP_URL_HOST));
var_dump(parse_url('[1::1]:80/foo.php', PHP_URL_PORT));
var_dump(parse_url('[1::1]:80/foo.php', PHP_URL_PATH));
var_dump(parse_url('http://[1::1]:80/foo.php', PHP_URL_SCHEME));
var_dump(parse_url('http://[1::1]:80/foo.php', PHP_URL_HOST));
var_dump(parse_url('http://[1::1]:80/foo.php', PHP_URL_PORT));
var_dump(parse_url('http://[1::1]:80/foo.php', PHP_URL_PATH));

var_dump(parse_url('[a::1]/foo.php', PHP_URL_HOST));
var_dump(parse_url('[a::1]/foo.php', PHP_URL_PATH));
var_dump(parse_url('[a::1]:80/foo.php', PHP_URL_HOST));
var_dump(parse_url('[a::1]:80/foo.php', PHP_URL_PORT));
var_dump(parse_url('[a::1]:80/foo.php', PHP_URL_PATH));
var_dump(parse_url('http://[a::1]:80/foo.php', PHP_URL_SCHEME));
var_dump(parse_url('http://[a::1]:80/foo.php', PHP_URL_HOST));
var_dump(parse_url('http://[a::1]:80/foo.php', PHP_URL_PORT));
var_dump(parse_url('http://[a::1]:80/foo.php', PHP_URL_PATH));
echo "Done"
?>
--EXPECTF--
string(%d) "[::1]"
string(%d) "/foo.php"
string(%d) "[::1]"
int(80)
string(%d) "/foo.php"
string(%d) "http"
string(%d) "[::1]"
int(80)
string(%d) "/foo.php"
string(%d) "[1::1]"
string(%d) "/foo.php"
string(%d) "[1::1]"
int(80)
string(%d) "/foo.php"
string(%d) "http"
string(%d) "[1::1]"
int(80)
string(%d) "/foo.php"
string(%d) "[a::1]"
string(%d) "/foo.php"
string(%d) "[a::1]"
int(80)
string(%d) "/foo.php"
string(%d) "http"
string(%d) "[a::1]"
int(80)
string(%d) "/foo.php"
Done
