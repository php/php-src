--TEST--
Test Uri\QueryParams::append() - success - empty names with null, empty, and non-empty values
--FILE--
<?php

$params = new Uri\QueryParams();
$params = $params->append("", null)->append("", "")->append("", "bar")->append("", null);

var_dump($params->toList());
var_dump($params->toRfc1866String());
var_dump($params->toRfc3986String());
var_dump($params->toWhatWgUrlString());

?>
--EXPECT--
array(4) {
  [0]=>
  array(2) {
    [0]=>
    string(0) ""
    [1]=>
    NULL
  }
  [1]=>
  array(2) {
    [0]=>
    string(0) ""
    [1]=>
    string(0) ""
  }
  [2]=>
  array(2) {
    [0]=>
    string(0) ""
    [1]=>
    string(3) "bar"
  }
  [3]=>
  array(2) {
    [0]=>
    string(0) ""
    [1]=>
    NULL
  }
}
string(8) "&=&=bar&"
string(8) "&=&=bar&"
string(6) "=&=bar"
