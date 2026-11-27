--TEST--
Test Uri\QueryParams::delete() - success - compacting a fully deleted list
--FILE--
<?php

$params1 = Uri\QueryParams::parseRfc3986(str_repeat("x=1&", 100) . "x=1");
$params2 = $params1->delete("x");

var_dump($params1->count());
var_dump($params2->toList());
var_dump(iterator_to_array($params2));
var_dump($params2->count());
var_dump($params2->has("x"));

$params2 = $params2->append("x", "2")->append("x", "3");
var_dump($params2->toRfc3986String());
var_dump($params2->count());
var_dump($params2->getAll("x"));
var_dump(iterator_to_array($params2) === $params2->toList());

?>
--EXPECT--
int(101)
array(0) {
}
array(0) {
}
int(0)
bool(false)
string(7) "x=2&x=3"
int(2)
array(2) {
  [0]=>
  string(1) "2"
  [1]=>
  string(1) "3"
}
bool(true)
