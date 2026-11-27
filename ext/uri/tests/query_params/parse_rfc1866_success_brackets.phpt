--TEST--
Test Uri\QueryParams::parseRfc1866() - success - unencoded brackets
--FILE--
<?php

$params = Uri\QueryParams::parseRfc1866("a[]=1");

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
    string(3) "a[]"
    [1]=>
    string(1) "1"
  }
}
string(9) "a%5B%5D=1"
string(9) "a%5B%5D=1"
string(9) "a%5B%5D=1"
