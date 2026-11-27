--TEST--
Test Uri\QueryParams::parseRfc1866() - success - an unencoded semicolon is not a parameter separator
--FILE--
<?php

$params = Uri\QueryParams::parseRfc1866("a=1;b=2");

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
    string(5) "1;b=2"
  }
}
string(11) "a=1%3Bb%3D2"
string(9) "a=1;b%3D2"
string(11) "a=1%3Bb%3D2"
