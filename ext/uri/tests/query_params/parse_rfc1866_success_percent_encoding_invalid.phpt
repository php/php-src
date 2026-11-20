--TEST--
Test Uri\QueryParams::parseRfc1866() - success - an invalid percent escape in a parameter value is preserved
--FILE--
<?php

$params = Uri\QueryParams::parseRfc1866("a=%0G");

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
    string(3) "%0G"
  }
}
string(7) "a=%250G"
string(7) "a=%250G"
string(7) "a=%250G"
