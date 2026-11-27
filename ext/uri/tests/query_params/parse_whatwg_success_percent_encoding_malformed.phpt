--TEST--
Test Uri\QueryParams::parseWhatWgUrlSearchParams() - success - malformed percent escapes remain literal
--FILE--
<?php

$params = Uri\QueryParams::parseWhatWgUrlSearchParams("%=x&foo=%2&bar=%GG");

var_dump($params->toList());
var_dump($params->toRfc1866String());
var_dump($params->toRfc3986String());
var_dump($params->toWhatWgUrlString());

?>
--EXPECT--
array(3) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "%"
    [1]=>
    string(1) "x"
  }
  [1]=>
  array(2) {
    [0]=>
    string(3) "foo"
    [1]=>
    string(2) "%2"
  }
  [2]=>
  array(2) {
    [0]=>
    string(3) "bar"
    [1]=>
    string(3) "%GG"
  }
}
string(24) "%25=x&foo=%252&bar=%25GG"
string(24) "%25=x&foo=%252&bar=%25GG"
string(24) "%25=x&foo=%252&bar=%25GG"
