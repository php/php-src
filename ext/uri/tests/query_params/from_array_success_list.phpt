--TEST--
Test Uri\QueryParams::fromArray() - success - scalar lists use empty brackets
--FILE--
<?php

$params = Uri\QueryParams::fromArray(["foo" => [1, 2, 3], "single" => [4]]);

var_dump($params->toList());
var_dump($params->toArray());

?>
--EXPECT--
array(4) {
  [0]=>
  array(2) {
    [0]=>
    string(5) "foo[]"
    [1]=>
    string(1) "1"
  }
  [1]=>
  array(2) {
    [0]=>
    string(5) "foo[]"
    [1]=>
    string(1) "2"
  }
  [2]=>
  array(2) {
    [0]=>
    string(5) "foo[]"
    [1]=>
    string(1) "3"
  }
  [3]=>
  array(2) {
    [0]=>
    string(8) "single[]"
    [1]=>
    string(1) "4"
  }
}
array(2) {
  ["foo"]=>
  array(3) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "2"
    [2]=>
    string(1) "3"
  }
  ["single"]=>
  array(1) {
    [0]=>
    string(1) "4"
  }
}
