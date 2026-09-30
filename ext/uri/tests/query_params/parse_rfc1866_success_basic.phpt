--TEST--
Test Uri\QueryParams::parseRfc1866() - success - basic
--FILE--
<?php

$params = Uri\QueryParams::parseRfc1866("abc=123&foo=bar");

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
    string(3) "abc"
    [1]=>
    string(3) "123"
  }
  [1]=>
  array(2) {
    [0]=>
    string(3) "foo"
    [1]=>
    string(3) "bar"
  }
}
string(15) "abc=123&foo=bar"
string(15) "abc=123&foo=bar"
string(15) "abc=123&foo=bar"
