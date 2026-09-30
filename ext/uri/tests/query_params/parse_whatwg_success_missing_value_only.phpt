--TEST--
Test Uri\QueryParams::parseWhatWgUrlSearchParams() - success - a name without a value
--FILE--
<?php

$params = Uri\QueryParams::parseWhatWgUrlSearchParams("foo");

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
    string(3) "foo"
    [1]=>
    string(0) ""
  }
}
string(4) "foo="
string(4) "foo="
string(4) "foo="
