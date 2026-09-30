--TEST--
Test Uri\QueryParams::append() - success - zero serialize_precision
--INI--
serialize_precision=0
--FILE--
<?php

$params = (new Uri\QueryParams())->append("x", 1.2345);

var_dump($params->toList());
var_dump($params->toRfc1866String());
var_dump($params->toRfc3986String());
var_dump($params->toWhatWgUrlString());

?>
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "x"
    [1]=>
    string(6) "1.0e+0"
  }
}
string(10) "x=1.0e%2B0"
string(8) "x=1.0e+0"
string(10) "x=1.0e%2B0"
