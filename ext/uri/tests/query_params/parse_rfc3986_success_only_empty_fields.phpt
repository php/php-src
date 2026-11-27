--TEST--
Test Uri\QueryParams::parseRfc3986() - success - a query consisting of empty fields
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("&");

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
    NULL
  }
  [1]=>
  array(2) {
    [0]=>
    string(0) ""
    [1]=>
    NULL
  }
}
string(1) "&"
string(1) "&"
string(0) ""
