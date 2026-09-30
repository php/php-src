--TEST--
Test Uri\QueryParams::fromArray() - success - integer and string backed enums
--FILE--
<?php

enum IntEnum: int { case Value = 123; }
enum StringEnum: string { case Value = "abc"; }
$params = Uri\QueryParams::fromArray(["integer" => IntEnum::Value, "string" => StringEnum::Value]);

var_dump($params->toList());
var_dump($params->toArray());

?>
--EXPECT--
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(7) "integer"
    [1]=>
    string(3) "123"
  }
  [1]=>
  array(2) {
    [0]=>
    string(6) "string"
    [1]=>
    string(3) "abc"
  }
}
array(2) {
  ["integer"]=>
  string(3) "123"
  ["string"]=>
  string(3) "abc"
}
