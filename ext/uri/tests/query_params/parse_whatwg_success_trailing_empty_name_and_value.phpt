--TEST--
Test Uri\QueryParams::parseWhatWgUrlSearchParams() - success - a trailing empty name and value is preserved
--FILE--
<?php

$params = Uri\QueryParams::parseWhatWgUrlSearchParams("foo=bar&=");

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
    string(3) "foo"
    [1]=>
    string(3) "bar"
  }
  [1]=>
  array(2) {
    [0]=>
    string(0) ""
    [1]=>
    string(0) ""
  }
}
string(9) "foo=bar&="
string(9) "foo=bar&="
string(9) "foo=bar&="
