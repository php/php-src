--TEST--
Test Uri\QueryParams::sortByBytes() - success - empty and fully deleted lists
--FILE--
<?php

$params1 = new Uri\QueryParams();
$params2 = $params1->sortByBytes();

var_dump($params1 === $params2);
var_dump($params2->toList());
var_dump($params2->count());

$params1 = (new Uri\QueryParams())->append("x", "1")->append("x", "2")->delete("x");
$params2 = $params1->sortByBytes();
var_dump($params2->toList());
var_dump($params2->count());

$params2 = $params2->append("z", "3")->append("a", "4");
var_dump($params1->toList());
var_dump($params2->toList());
var_dump(iterator_to_array($params2) === $params2->toList());
var_dump($params2->getFirst("z"));
var_dump($params2->getLast("a"));

?>
--EXPECT--
bool(false)
array(0) {
}
int(0)
array(0) {
}
int(0)
array(0) {
}
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "z"
    [1]=>
    string(1) "3"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "4"
  }
}
bool(true)
string(1) "3"
string(1) "4"
