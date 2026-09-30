--TEST--
Test Uri\QueryParams::fromArray() - success - omitted list elements leave gaps when indices are explicit
--FILE--
<?php

$params = Uri\QueryParams::fromArray(["k" => ["one", [], "two"]]);

var_dump($params->toList());
var_dump($params->toArray());

?>
--EXPECT--
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(4) "k[0]"
    [1]=>
    string(3) "one"
  }
  [1]=>
  array(2) {
    [0]=>
    string(4) "k[2]"
    [1]=>
    string(3) "two"
  }
}
array(1) {
  ["k"]=>
  array(2) {
    [0]=>
    string(3) "one"
    [2]=>
    string(3) "two"
  }
}
