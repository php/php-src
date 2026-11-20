--TEST--
Test Uri\QueryParams::parseRfc1866() - success - unencoded valid UTF-8
--FILE--
<?php

$params = Uri\QueryParams::parseRfc1866("a=\xC3\xA9");

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
    string(2) "é"
  }
}
string(8) "a=%C3%A9"
string(8) "a=%C3%A9"
string(8) "a=%C3%A9"
