--TEST--
Test Uri\QueryParams::withArray() - success - replaces the complete list without changing the original
--FILE--
<?php

$params1 = Uri\QueryParams::parseRfc3986("old=1&old=2&keep=3");
$params2 = $params1->withArray(["new" => ["a", "b"]]);

var_dump($params1 === $params2);
var_dump($params1->toRfc3986String());
var_dump($params2->toList());
var_dump($params2->toArray());
var_dump($params2->has("old"));
var_dump($params2->getAll("new[]"));
var_dump($params2->count());

?>
--EXPECT--
bool(false)
string(18) "old=1&old=2&keep=3"
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(5) "new[]"
    [1]=>
    string(1) "a"
  }
  [1]=>
  array(2) {
    [0]=>
    string(5) "new[]"
    [1]=>
    string(1) "b"
  }
}
array(1) {
  ["new"]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "b"
  }
}
bool(false)
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
}
int(2)
