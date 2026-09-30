--TEST--
Test Uri\QueryParams::fromArray() - success - root integer keys become parameter names
--FILE--
<?php

$params = Uri\QueryParams::fromArray([2 => "two", "01" => "leading", -1 => "negative", 0 => "zero"]);

var_dump($params->toList());
var_dump($params->toArray());

?>
--EXPECT--
array(4) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "2"
    [1]=>
    string(3) "two"
  }
  [1]=>
  array(2) {
    [0]=>
    string(2) "01"
    [1]=>
    string(7) "leading"
  }
  [2]=>
  array(2) {
    [0]=>
    string(2) "-1"
    [1]=>
    string(8) "negative"
  }
  [3]=>
  array(2) {
    [0]=>
    string(1) "0"
    [1]=>
    string(4) "zero"
  }
}
array(4) {
  [2]=>
  string(3) "two"
  ["01"]=>
  string(7) "leading"
  [-1]=>
  string(8) "negative"
  [0]=>
  string(4) "zero"
}
