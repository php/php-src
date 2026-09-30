--TEST--
Test Uri\QueryParams::parseRfc1866() - success - an invalid percent escape in a parameter name is preserved
--FILE--
<?php

$params = Uri\QueryParams::parseRfc1866("ok=1&%G0=value");

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
    string(2) "ok"
    [1]=>
    string(1) "1"
  }
  [1]=>
  array(2) {
    [0]=>
    string(3) "%G0"
    [1]=>
    string(5) "value"
  }
}
string(16) "ok=1&%25G0=value"
string(16) "ok=1&%25G0=value"
string(16) "ok=1&%25G0=value"
