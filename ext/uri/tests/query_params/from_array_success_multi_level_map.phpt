--TEST--
Test Uri\QueryParams::fromArray() - success - nested maps preserve explicit indices
--FILE--
<?php

$params = Uri\QueryParams::fromArray(["foo" => ["bar" => [0 => "baz", 2 => 123, 3 => [], 4 => [1, 2, 3]]]]);

var_dump($params->toList());
var_dump($params->toArray());

?>
--EXPECT--
array(5) {
  [0]=>
  array(2) {
    [0]=>
    string(11) "foo[bar][0]"
    [1]=>
    string(3) "baz"
  }
  [1]=>
  array(2) {
    [0]=>
    string(11) "foo[bar][2]"
    [1]=>
    string(3) "123"
  }
  [2]=>
  array(2) {
    [0]=>
    string(13) "foo[bar][4][]"
    [1]=>
    string(1) "1"
  }
  [3]=>
  array(2) {
    [0]=>
    string(13) "foo[bar][4][]"
    [1]=>
    string(1) "2"
  }
  [4]=>
  array(2) {
    [0]=>
    string(13) "foo[bar][4][]"
    [1]=>
    string(1) "3"
  }
}
array(1) {
  ["foo"]=>
  array(1) {
    ["bar"]=>
    array(3) {
      [0]=>
      string(3) "baz"
      [2]=>
      string(3) "123"
      [4]=>
      array(3) {
        [0]=>
        string(1) "1"
        [1]=>
        string(1) "2"
        [2]=>
        string(1) "3"
      }
    }
  }
}
