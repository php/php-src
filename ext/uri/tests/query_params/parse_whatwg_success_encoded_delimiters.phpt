--TEST--
Test Uri\QueryParams::parseWhatWgUrlSearchParams() - success - percent-encoded parameter delimiters
--FILE--
<?php

$params = Uri\QueryParams::parseWhatWgUrlSearchParams("%26a%3Db%26=%3Dc%26d%3D&x=%2526");

var_dump($params->toList());
var_dump($params->toRfc1866String());
var_dump($params->toRfc3986String());
var_dump($params->toWhatWgUrlString());

?>
--EXPECT--
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(5) "&a=b&"
    [1]=>
    string(5) "=c&d="
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "x"
    [1]=>
    string(3) "%26"
  }
}
string(31) "%26a%3Db%26=%3Dc%26d%3D&x=%2526"
string(31) "%26a%3Db%26=%3Dc%26d%3D&x=%2526"
string(31) "%26a%3Db%26=%3Dc%26d%3D&x=%2526"
