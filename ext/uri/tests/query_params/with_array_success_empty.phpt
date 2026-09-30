--TEST--
Test Uri\QueryParams::withArray() - success - clears a list containing tombstones
--FILE--
<?php

$params1 = Uri\QueryParams::parseRfc3986("old=1&old=2&keep=3")->delete("old");
$params2 = $params1->withArray([]);

var_dump($params1->toList());
var_dump($params2->toList());
var_dump($params2->count());
$params2 = $params2->append("new", "4");
var_dump($params2->toList());
var_dump($params2->getFirst("new"));

?>
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(4) "keep"
    [1]=>
    string(1) "3"
  }
}
array(0) {
}
int(0)
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(3) "new"
    [1]=>
    string(1) "4"
  }
}
string(1) "4"
