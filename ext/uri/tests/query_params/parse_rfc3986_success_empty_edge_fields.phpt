--TEST--
Test Uri\QueryParams::parseRfc3986() - success - leading and trailing empty fields
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("&foo=bar&");

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
    string(0) ""
    [1]=>
    NULL
  }
  [1]=>
  array(2) {
    [0]=>
    string(3) "foo"
    [1]=>
    string(3) "bar"
  }
  [2]=>
  array(2) {
    [0]=>
    string(0) ""
    [1]=>
    NULL
  }
}
string(9) "&foo=bar&"
string(9) "&foo=bar&"
string(7) "foo=bar"
