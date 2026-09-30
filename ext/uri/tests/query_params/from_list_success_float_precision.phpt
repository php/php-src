--TEST--
Test Uri\QueryParams::fromList() - success - preserve float precision
--INI--
precision=3
serialize_precision=-1
--FILE--
<?php

$value = 1.2345678901234567;
$params = Uri\QueryParams::fromList([["x", $value]]);

var_dump($params->toList());
var_dump($params->toRfc3986String());
var_dump(floatval($params->getFirst("x")) === $value);

?>
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "x"
    [1]=>
    string(18) "1.2345678901234567"
  }
}
string(20) "x=1.2345678901234567"
bool(true)
