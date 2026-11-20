--TEST--
Test Uri\QueryParams::fromArray() - success - nested lists retain their grouping
--FILE--
<?php

$params = Uri\QueryParams::fromArray(["foo" => [["bar"], [123]]]);

var_dump($params->toList());
var_dump($params->toArray());

?>
--EXPECT--
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(8) "foo[0][]"
    [1]=>
    string(3) "bar"
  }
  [1]=>
  array(2) {
    [0]=>
    string(8) "foo[1][]"
    [1]=>
    string(3) "123"
  }
}
array(1) {
  ["foo"]=>
  array(2) {
    [0]=>
    array(1) {
      [0]=>
      string(3) "bar"
    }
    [1]=>
    array(1) {
      [0]=>
      string(3) "123"
    }
  }
}
