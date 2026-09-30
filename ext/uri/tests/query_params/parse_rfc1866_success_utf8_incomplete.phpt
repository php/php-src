--TEST--
Test Uri\QueryParams::parseRfc1866() - success - incomplete percent-encoded UTF-8
--FILE--
<?php

$params = Uri\QueryParams::parseRfc1866("a%E2%82=b%E2%82");

var_dump($params->toList());
var_dump($params->toRfc1866String());
var_dump($params->toRfc3986String());
var_dump($params->toWhatWgUrlString());

?>
--EXPECTF--
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(3) "%r\x61\xE2\x82%r"
    [1]=>
    string(3) "%r\x62\xE2\x82%r"
  }
}
string(15) "a%E2%82=b%E2%82"
string(15) "a%E2%82=b%E2%82"
string(21) "a%EF%BF%BD=b%EF%BF%BD"
