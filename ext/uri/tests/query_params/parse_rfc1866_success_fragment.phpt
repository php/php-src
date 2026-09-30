--TEST--
Test Uri\QueryParams::parseRfc1866() - success - an unencoded fragment delimiter is preserved
--FILE--
<?php

$params = Uri\QueryParams::parseRfc1866("ok=1&a=#value");

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
    string(1) "a"
    [1]=>
    string(6) "#value"
  }
}
string(15) "ok=1&a=%23value"
string(15) "ok=1&a=%23value"
string(15) "ok=1&a=%23value"
