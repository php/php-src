--TEST--
Test Uri\QueryParams::parseWhatWgUrlSearchParams() - success - an empty field between parameters
--FILE--
<?php

$params = Uri\QueryParams::parseWhatWgUrlSearchParams("a=1&&b=1");

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
    string(1) "a"
    [1]=>
    string(1) "1"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "b"
    [1]=>
    string(1) "1"
  }
}
string(7) "a=1&b=1"
string(7) "a=1&b=1"
string(7) "a=1&b=1"
