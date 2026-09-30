--TEST--
Test Uri\QueryParams::parseRfc3986() - success - param count equals the limit
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("a=1&a=2", 2);

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
    string(1) "1"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "2"
  }
}
string(7) "a=1&a=2"
string(7) "a=1&a=2"
string(7) "a=1&a=2"
