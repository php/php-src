--TEST--
Test Uri\QueryParams::fromArray() - success - configured serialize_precision
--INI--
precision=3
serialize_precision=5
--FILE--
<?php

$params = Uri\QueryParams::fromArray(["x" => 1.2345678901234567]);

var_dump($params->toList());
var_dump($params->toRfc3986String());

?>
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "x"
    [1]=>
    string(6) "1.2346"
  }
}
string(8) "x=1.2346"
