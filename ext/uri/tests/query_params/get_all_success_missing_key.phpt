--TEST--
Test Uri\QueryParams::getAll() - success - parameter names are matched literally
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("x%5B%5D=1&x%5B%5D=2");

var_dump($params->getAll("x"));
var_dump($params->getAll("x[]"));

?>
--EXPECT--
array(0) {
}
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "2"
}
