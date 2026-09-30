--TEST--
Test Uri\QueryParams serialization - success - duplicate names and deleted entries
--FILE--
<?php

$params1 = Uri\QueryParams::parseRfc3986("x=1&discard=no&flag&x=2&empty=")->delete("discard");
$params2 = unserialize(serialize($params1));

var_dump($params1 === $params2);
var_dump($params2->toList() === $params1->toList());
var_dump($params2->getAll("x"));
var_dump($params2->count());

$params2 = $params2->append("x", "3")->set("flag", "yes")->deleteValue("x", "1");
var_dump($params1->toRfc3986String());
var_dump($params2->toRfc3986String());
var_dump($params2->getFirst("x"));
var_dump($params2->getLast("x"));
var_dump($params2->count());

?>
--EXPECT--
bool(false)
bool(true)
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "2"
}
int(4)
string(19) "x=1&flag&x=2&empty="
string(23) "flag=yes&x=2&empty=&x=3"
string(1) "2"
string(1) "3"
int(4)
