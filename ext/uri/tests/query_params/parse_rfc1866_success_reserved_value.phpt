--TEST--
Test Uri\QueryParams::parseRfc1866() - success - an unencoded reserved character in a parameter value
--FILE--
<?php

$params = Uri\QueryParams::parseRfc1866("foo=a/b");

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
    string(3) "a/b"
  }
}
string(9) "foo=a%2Fb"
string(7) "foo=a/b"
string(9) "foo=a%2Fb"
