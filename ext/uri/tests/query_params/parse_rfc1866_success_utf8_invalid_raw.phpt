--TEST--
Test Uri\QueryParams::parseRfc1866() - success - unencoded invalid UTF-8 is preserved
--FILE--
<?php

$params = Uri\QueryParams::parseRfc1866("a=\xFF");

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
    string(1) "a"
    [1]=>
    string(1) "%r\xFF%r"
  }
}
string(5) "a=%FF"
string(5) "a=%FF"
string(11) "a=%EF%BF%BD"
