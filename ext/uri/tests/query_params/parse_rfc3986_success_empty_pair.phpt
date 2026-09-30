--TEST--
Test Uri\QueryParams::parseRfc3986() - success - empty query param pair
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("a=1&&b=1");

var_dump($params->toList());
var_dump($params->toRfc1866String());
var_dump($params->toRfc3986String());
var_dump($params->toWhatWgUrlString());

?>
--EXPECT--
array(3) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "1"
  }
  [1]=>
  array(2) {
    [0]=>
    string(0) ""
    [1]=>
    NULL
  }
  [2]=>
  array(2) {
    [0]=>
    string(1) "b"
    [1]=>
    string(1) "1"
  }
}
string(8) "a=1&&b=1"
string(8) "a=1&&b=1"
string(7) "a=1&b=1"
