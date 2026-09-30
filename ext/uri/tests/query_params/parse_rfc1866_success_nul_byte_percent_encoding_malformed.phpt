--TEST--
Test Uri\QueryParams::parseRfc1866() - success - NUL bytes next to malformed percent escapes
--FILE--
<?php

$params = Uri\QueryParams::parseRfc1866("a\0%=b\0%2&c=%\0&d=%2\0&next=ok");

var_dump($params->toList());
var_dump($params->toRfc1866String());
var_dump($params->toRfc3986String());
var_dump($params->toWhatWgUrlString());

?>
--EXPECTF--
array(4) {
  [0]=>
  array(2) {
    [0]=>
    string(3) "a%0%r\x25%r"
    [1]=>
    string(4) "b%0%r\x25%r2"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "c"
    [1]=>
    string(2) "%r\x25%r%0"
  }
  [2]=>
  array(2) {
    [0]=>
    string(1) "d"
    [1]=>
    string(3) "%r\x25%r2%0"
  }
  [3]=>
  array(2) {
    [0]=>
    string(4) "next"
    [1]=>
    string(2) "ok"
  }
}
string(43) "a%r\x25%r00%r\x25%r25=b%r\x25%r00%r\x25%r252&c=%r\x25%r25%r\x25%r00&d=%r\x25%r252%r\x25%r00&next=ok"
string(43) "a%r\x25%r00%r\x25%r25=b%r\x25%r00%r\x25%r252&c=%r\x25%r25%r\x25%r00&d=%r\x25%r252%r\x25%r00&next=ok"
string(43) "a%r\x25%r00%r\x25%r25=b%r\x25%r00%r\x25%r252&c=%r\x25%r25%r\x25%r00&d=%r\x25%r252%r\x25%r00&next=ok"
