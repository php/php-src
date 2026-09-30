--TEST--
Test Uri\QueryParams::parseRfc1866() - success - NUL bytes before and after plus-to-space conversion
--FILE--
<?php

$params = Uri\QueryParams::parseRfc1866("a+\0b=c\0+d&next=ok");

var_dump($params->toList());
var_dump($params->toRfc1866String());
var_dump($params->toRfc3986String());
var_dump($params->toWhatWgUrlString());

?>
--EXPECTF--
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(4) "a %0b"
    [1]=>
    string(4) "c%0 d"
  }
  [1]=>
  array(2) {
    [0]=>
    string(4) "next"
    [1]=>
    string(2) "ok"
  }
}
string(21) "a+%r\x25%r00b=c%r\x25%r00+d&next=ok"
string(25) "a%r\x25%r20%r\x25%r00b=c%r\x25%r00%r\x25%r20d&next=ok"
string(21) "a+%r\x25%r00b=c%r\x25%r00+d&next=ok"
