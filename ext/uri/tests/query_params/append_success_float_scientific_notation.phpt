--TEST--
Test Uri\QueryParams::append() - success - scientific notation of floats
--INI--
precision=3
serialize_precision=-1
--FILE--
<?php

$params = (new Uri\QueryParams())->append("small", 1e-10)->append("large", 1e20)->append("integer", 1e14);

var_dump($params->toList());
var_dump($params->toRfc1866String());
var_dump($params->toRfc3986String());
var_dump($params->toWhatWgUrlString());
var_dump(floatval($params->getFirst("small")) === 1e-10);
var_dump(floatval($params->getFirst("large")) === 1e20);
var_dump(floatval($params->getFirst("integer")) === 1e14);

?>
--EXPECT--
array(3) {
  [0]=>
  array(2) {
    [0]=>
    string(5) "small"
    [1]=>
    string(7) "1.0e-10"
  }
  [1]=>
  array(2) {
    [0]=>
    string(5) "large"
    [1]=>
    string(7) "1.0e+20"
  }
  [2]=>
  array(2) {
    [0]=>
    string(7) "integer"
    [1]=>
    string(15) "100000000000000"
  }
}
string(53) "small=1.0e-10&large=1.0e%2B20&integer=100000000000000"
string(51) "small=1.0e-10&large=1.0e+20&integer=100000000000000"
string(53) "small=1.0e-10&large=1.0e%2B20&integer=100000000000000"
bool(true)
bool(true)
bool(true)
