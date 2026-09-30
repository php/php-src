--TEST--
Test Uri\QueryParams::fromArray() - success - mixed map keys and value types
--FILE--
<?php

$params = Uri\QueryParams::fromArray(["baz" => [null, 0, "key2" => 1, 123, "", ["bla"]]]);

var_dump($params->toList());
var_dump($params->toArray());

?>
--EXPECT--
array(6) {
  [0]=>
  array(2) {
    [0]=>
    string(6) "baz[0]"
    [1]=>
    NULL
  }
  [1]=>
  array(2) {
    [0]=>
    string(6) "baz[1]"
    [1]=>
    string(1) "0"
  }
  [2]=>
  array(2) {
    [0]=>
    string(9) "baz[key2]"
    [1]=>
    string(1) "1"
  }
  [3]=>
  array(2) {
    [0]=>
    string(6) "baz[2]"
    [1]=>
    string(3) "123"
  }
  [4]=>
  array(2) {
    [0]=>
    string(6) "baz[3]"
    [1]=>
    string(0) ""
  }
  [5]=>
  array(2) {
    [0]=>
    string(8) "baz[4][]"
    [1]=>
    string(3) "bla"
  }
}
array(1) {
  ["baz"]=>
  array(6) {
    [0]=>
    NULL
    [1]=>
    string(1) "0"
    ["key2"]=>
    string(1) "1"
    [2]=>
    string(3) "123"
    [3]=>
    string(0) ""
    [4]=>
    array(1) {
      [0]=>
      string(3) "bla"
    }
  }
}
