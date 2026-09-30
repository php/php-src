--TEST--
Test Uri\QueryParams::parseRfc1866() - success - an unencoded NUL byte
--FILE--
<?php

$params = Uri\QueryParams::parseRfc1866("a=x\0y");

var_dump($params->toList());
var_dump($params->toRfc1866String());
var_dump($params->toRfc3986String());
var_dump($params->toWhatWgUrlString());

?>
--EXPECTF--
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    string(3) "x%0y"
  }
}
string(7) "a=x%r\x25%r00y"
string(7) "a=x%r\x25%r00y"
string(7) "a=x%r\x25%r00y"
