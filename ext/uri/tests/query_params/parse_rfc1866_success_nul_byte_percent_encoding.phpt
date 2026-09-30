--TEST--
Test Uri\QueryParams::parseRfc1866() - success - NUL bytes before and after percent-decoding
--FILE--
<?php

$params = Uri\QueryParams::parseRfc1866("%61\0%62=%63\0%64&%00x=y%00&next=ok");

var_dump($params->toList());
var_dump($params->toRfc1866String());
var_dump($params->toRfc3986String());
var_dump($params->toWhatWgUrlString());

?>
--EXPECTF--
array(3) {
  [0]=>
  array(2) {
    [0]=>
    string(3) "a%0b"
    [1]=>
    string(3) "c%0d"
  }
  [1]=>
  array(2) {
    [0]=>
    string(2) "%0x"
    [1]=>
    string(2) "y%0"
  }
  [2]=>
  array(2) {
    [0]=>
    string(4) "next"
    [1]=>
    string(2) "ok"
  }
}
string(29) "a%r\x25%r00b=c%r\x25%r00d&%r\x25%r00x=y%r\x25%r00&next=ok"
string(29) "a%r\x25%r00b=c%r\x25%r00d&%r\x25%r00x=y%r\x25%r00&next=ok"
string(29) "a%r\x25%r00b=c%r\x25%r00d&%r\x25%r00x=y%r\x25%r00&next=ok"
