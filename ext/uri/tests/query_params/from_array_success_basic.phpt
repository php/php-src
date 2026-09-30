--TEST--
Test Uri\QueryParams::fromArray() - success - scalar values
--FILE--
<?php

$params = Uri\QueryParams::fromArray(["foo" => "bar", "number" => 123, "float" => 3.5, "flag" => null, "empty" => ""]);

var_dump($params->toList());
var_dump($params->toArray());
var_dump($params->toRfc3986String());

?>
--EXPECT--
array(5) {
  [0]=>
  array(2) {
    [0]=>
    string(3) "foo"
    [1]=>
    string(3) "bar"
  }
  [1]=>
  array(2) {
    [0]=>
    string(6) "number"
    [1]=>
    string(3) "123"
  }
  [2]=>
  array(2) {
    [0]=>
    string(5) "float"
    [1]=>
    string(3) "3.5"
  }
  [3]=>
  array(2) {
    [0]=>
    string(4) "flag"
    [1]=>
    NULL
  }
  [4]=>
  array(2) {
    [0]=>
    string(5) "empty"
    [1]=>
    string(0) ""
  }
}
array(5) {
  ["foo"]=>
  string(3) "bar"
  ["number"]=>
  string(3) "123"
  ["float"]=>
  string(3) "3.5"
  ["flag"]=>
  NULL
  ["empty"]=>
  string(0) ""
}
string(40) "foo=bar&number=123&float=3.5&flag&empty="
