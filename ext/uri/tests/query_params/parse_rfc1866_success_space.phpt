--TEST--
Test Uri\QueryParams::parseRfc1866() - success - an unencoded space
--FILE--
<?php

$params = Uri\QueryParams::parseRfc1866("a=foo bar");

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
    string(1) "a"
    [1]=>
    string(7) "foo bar"
  }
}
string(9) "a=foo+bar"
string(11) "a=foo%20bar"
string(9) "a=foo+bar"
