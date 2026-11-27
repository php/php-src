--TEST--
Test Uri\QueryParams::deleteValue() - success - later matching value in a temporary instance
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("x=1&x=2&x=3&x=2")->deleteValue("x", "2");

var_dump($params->getAll("x"));
var_dump($params->getFirst("x"));
var_dump($params->getLast("x"));
var_dump($params->count());
var_dump($params->toRfc3986String());

?>
--EXPECT--
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "3"
}
string(1) "1"
string(1) "3"
int(2)
string(7) "x=1&x=3"
