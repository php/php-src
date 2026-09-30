--TEST--
Test Uri\QueryParams::parseRfc1866() - success - null value
--FILE--
<?php

$params = Uri\QueryParams::parseRfc1866("a&b=c");

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
    string(1) "a"
    [1]=>
    NULL
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "b"
    [1]=>
    string(1) "c"
  }
}
string(5) "a&b=c"
string(5) "a&b=c"
string(5) "a&b=c"
