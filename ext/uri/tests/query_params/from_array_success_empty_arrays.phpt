--TEST--
Test Uri\QueryParams::fromArray() - success - empty arrays are omitted at every level
--FILE--
<?php

$params = Uri\QueryParams::fromArray(["foo" => "bar", "empty" => [], "nested" => ["empty" => []]]);

var_dump($params->toList());
var_dump($params->toArray());

?>
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(3) "foo"
    [1]=>
    string(3) "bar"
  }
}
array(1) {
  ["foo"]=>
  string(3) "bar"
}
