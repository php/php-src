--TEST--
Test Uri\QueryParams::fromArray() - success - shared arrays and scalar references
--FILE--
<?php

$scalar = 7;
$shared = ["value" => &$scalar];
$params = Uri\QueryParams::fromArray(["left" => &$shared, "right" => &$shared, "list" => [&$scalar, &$shared]]);

var_dump($params->toList());
var_dump($params->toArray());

?>
--EXPECT--
array(4) {
  [0]=>
  array(2) {
    [0]=>
    string(11) "left[value]"
    [1]=>
    string(1) "7"
  }
  [1]=>
  array(2) {
    [0]=>
    string(12) "right[value]"
    [1]=>
    string(1) "7"
  }
  [2]=>
  array(2) {
    [0]=>
    string(7) "list[0]"
    [1]=>
    string(1) "7"
  }
  [3]=>
  array(2) {
    [0]=>
    string(14) "list[1][value]"
    [1]=>
    string(1) "7"
  }
}
array(3) {
  ["left"]=>
  array(1) {
    ["value"]=>
    string(1) "7"
  }
  ["right"]=>
  array(1) {
    ["value"]=>
    string(1) "7"
  }
  ["list"]=>
  array(2) {
    [0]=>
    string(1) "7"
    [1]=>
    array(1) {
      ["value"]=>
      string(1) "7"
    }
  }
}
