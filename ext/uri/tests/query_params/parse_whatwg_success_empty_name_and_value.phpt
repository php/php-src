--TEST--
Test Uri\QueryParams::parseWhatWgUrlSearchParams() - success - an empty name and value
--FILE--
<?php

$params = Uri\QueryParams::parseWhatWgUrlSearchParams("=");

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
    string(0) ""
    [1]=>
    string(0) ""
  }
}
string(1) "="
string(1) "="
string(1) "="
