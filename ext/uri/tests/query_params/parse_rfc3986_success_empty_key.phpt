--TEST--
Test Uri\QueryParams::parseRfc3986() - success - empty query param pair
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("=1&=2");

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
    string(0) ""
    [1]=>
    string(1) "1"
  }
  [1]=>
  array(2) {
    [0]=>
    string(0) ""
    [1]=>
    string(1) "2"
  }
}
string(5) "=1&=2"
string(5) "=1&=2"
string(5) "=1&=2"
