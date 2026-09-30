--TEST--
Test Uri\QueryParams::parseWhatWgUrlSearchParams() - success - leading and trailing empty fields
--FILE--
<?php

$params = Uri\QueryParams::parseWhatWgUrlSearchParams("&foo=bar&");

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
    string(3) "foo"
    [1]=>
    string(3) "bar"
  }
}
string(7) "foo=bar"
string(7) "foo=bar"
string(7) "foo=bar"
