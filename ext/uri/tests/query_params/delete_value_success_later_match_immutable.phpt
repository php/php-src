--TEST--
Test Uri\QueryParams::deleteValue() - success - deleting later matching values leaves the original instance unchanged
--FILE--
<?php

$params1 = Uri\QueryParams::parseRfc3986("x=1&other=keep&x=2&x=3&x=2");
$params2 = $params1->deleteValue("x", "2");

var_dump($params1 === $params2);
var_dump($params1->toRfc3986String());
var_dump($params1->getAll("x"));
var_dump($params2->toRfc3986String());
var_dump($params2->getAll("x"));
var_dump($params2->getFirst("x"));
var_dump($params2->getLast("x"));
var_dump($params2->count());

?>
--EXPECT--
bool(false)
string(26) "x=1&other=keep&x=2&x=3&x=2"
array(4) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "2"
  [2]=>
  string(1) "3"
  [3]=>
  string(1) "2"
}
string(18) "x=1&other=keep&x=3"
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "3"
}
string(1) "1"
string(1) "3"
int(3)
