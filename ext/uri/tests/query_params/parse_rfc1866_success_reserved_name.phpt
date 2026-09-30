--TEST--
Test Uri\QueryParams::parseRfc1866() - success - an unencoded reserved character in a parameter name
--FILE--
<?php

$params = Uri\QueryParams::parseRfc1866("foo:bar=value");

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
    string(7) "foo:bar"
    [1]=>
    string(5) "value"
  }
}
string(15) "foo%3Abar=value"
string(13) "foo:bar=value"
string(15) "foo%3Abar=value"
