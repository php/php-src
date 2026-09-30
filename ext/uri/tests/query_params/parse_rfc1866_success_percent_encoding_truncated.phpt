--TEST--
Test Uri\QueryParams::parseRfc1866() - success - a percent escape with only one hexadecimal digit is preserved
--FILE--
<?php

$params = Uri\QueryParams::parseRfc1866("a=%2");

var_dump($params->toList());
var_dump($params->toRfc1866String());
var_dump($params->toRfc3986String());
var_dump($params->toWhatWgUrlString());

?>
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    string(2) "%2"
  }
}
string(6) "a=%252"
string(6) "a=%252"
string(6) "a=%252"
