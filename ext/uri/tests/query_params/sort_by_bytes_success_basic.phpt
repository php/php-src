--TEST--
Test Uri\QueryParams::sortByBytes() - success - stable, case-sensitive sorting and immutability
--FILE--
<?php

$params1 = (new Uri\QueryParams())
    ->append("z", "6")->append("b", "6")->append("A", "upper")->append("10", "ten")
    ->append("z", "5")->append("b", "5")->append("2", "two")
    ->append("z", "4")->append("b", "4")->append("z", "3")->append("b", "3")
    ->append("z", "2")->append("b", "2")->append("z", "1")->append("b", "1")
    ->append("a", "lower")->append("B", "upper")->append("0", "zero")->append("b", null);
$params2 = $params1->sortByBytes();

var_dump($params1 === $params2);
var_dump($params1->toRfc3986String());
var_dump($params2->toRfc3986String());
var_dump($params2->count());
var_dump($params2->getAll("b"));
var_dump($params2->getFirst("b"));
var_dump($params2->getLast("b"));
var_dump(iterator_to_array($params2) === $params2->toList());
var_dump($params2->sortByBytes()->toRfc3986String());

?>
--EXPECT--
bool(false)
string(93) "z=6&b=6&A=upper&10=ten&z=5&b=5&2=two&z=4&b=4&z=3&b=3&z=2&b=2&z=1&b=1&a=lower&B=upper&0=zero&b"
string(93) "0=zero&10=ten&2=two&A=upper&B=upper&a=lower&b=6&b=5&b=4&b=3&b=2&b=1&b&z=6&z=5&z=4&z=3&z=2&z=1"
int(19)
array(7) {
  [0]=>
  string(1) "6"
  [1]=>
  string(1) "5"
  [2]=>
  string(1) "4"
  [3]=>
  string(1) "3"
  [4]=>
  string(1) "2"
  [5]=>
  string(1) "1"
  [6]=>
  NULL
}
string(1) "6"
NULL
bool(true)
string(93) "0=zero&10=ten&2=two&A=upper&B=upper&a=lower&b=6&b=5&b=4&b=3&b=2&b=1&b&z=6&z=5&z=4&z=3&z=2&z=1"
