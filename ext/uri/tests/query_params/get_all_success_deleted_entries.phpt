--TEST--
Test Uri\QueryParams::getAll() - success - deleted entries and independent returned arrays
--FILE--
<?php

$params1 = Uri\QueryParams::parseRfc3986("x=1&x=2&x=1&x=3");
$params2 = $params1->deleteValue("x", "1");
$values = $params2->getAll("x");
$values[0] = "changed";

var_dump($params1->getAll("x"));
var_dump($params2->getAll("x"));

?>
--EXPECT--
array(4) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "2"
  [2]=>
  string(1) "1"
  [3]=>
  string(1) "3"
}
array(2) {
  [0]=>
  string(1) "2"
  [1]=>
  string(1) "3"
}
