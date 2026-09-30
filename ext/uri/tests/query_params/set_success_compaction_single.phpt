--TEST--
Test Uri\QueryParams::set() - success - compacting to a single entry
--FILE--
<?php

$params1 = Uri\QueryParams::parseRfc3986(str_repeat("x=1&", 101) . "x=1");
$params2 = $params1->set("x", "2");

var_dump($params1->count());
var_dump($params1->getFirst("x"));
var_dump($params2->toRfc3986String());
var_dump($params2->count());
var_dump($params2->getFirst("x"));
var_dump($params2->getLast("x"));
var_dump(iterator_to_array($params2) === $params2->toList());

$params2 = $params2->append("x", "3");
var_dump($params2->toRfc3986String());
var_dump($params2->getAll("x"));
var_dump(iterator_to_array($params2) === $params2->toList());

?>
--EXPECT--
int(102)
string(1) "1"
string(3) "x=2"
int(1)
string(1) "2"
string(1) "2"
bool(true)
string(7) "x=2&x=3"
array(2) {
  [0]=>
  string(1) "2"
  [1]=>
  string(1) "3"
}
bool(true)
