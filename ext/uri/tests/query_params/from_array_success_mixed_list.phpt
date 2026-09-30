--TEST--
Test Uri\QueryParams::fromArray() - success - a list with an array uses explicit indices for all elements
--FILE--
<?php

$params = Uri\QueryParams::fromArray(["k" => [1, ["a" => 2], 3]]);

var_dump($params->toList());
var_dump($params->toArray());

?>
--EXPECT--
array(3) {
  [0]=>
  array(2) {
    [0]=>
    string(4) "k[0]"
    [1]=>
    string(1) "1"
  }
  [1]=>
  array(2) {
    [0]=>
    string(7) "k[1][a]"
    [1]=>
    string(1) "2"
  }
  [2]=>
  array(2) {
    [0]=>
    string(4) "k[2]"
    [1]=>
    string(1) "3"
  }
}
array(1) {
  ["k"]=>
  array(3) {
    [0]=>
    string(1) "1"
    [1]=>
    array(1) {
      ["a"]=>
      string(1) "2"
    }
    [2]=>
    string(1) "3"
  }
}
