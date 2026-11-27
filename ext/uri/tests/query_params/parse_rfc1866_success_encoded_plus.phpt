--TEST--
Test Uri\QueryParams::parseRfc1866() - success - percent-encoded plus signs remain literal
--FILE--
<?php

$params = Uri\QueryParams::parseRfc1866("a%2Bb=c%2Bd+e");

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
    string(3) "a+b"
    [1]=>
    string(5) "c+d e"
  }
}
string(13) "a%2Bb=c%2Bd+e"
string(11) "a+b=c+d%20e"
string(13) "a%2Bb=c%2Bd+e"
