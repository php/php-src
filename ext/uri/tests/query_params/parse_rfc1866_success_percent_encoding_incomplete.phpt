--TEST--
Test Uri\QueryParams::parseRfc1866() - success - an incomplete percent escape is preserved
--FILE--
<?php

$params = Uri\QueryParams::parseRfc1866("a=%");

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
    string(1) "%"
  }
}
string(5) "a=%25"
string(5) "a=%25"
string(5) "a=%25"
