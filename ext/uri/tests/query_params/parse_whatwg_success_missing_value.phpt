--TEST--
Test Uri\QueryParams::parseWhatWgUrlSearchParams() - success - missing value becomes an empty string
--FILE--
<?php

$params = Uri\QueryParams::parseWhatWgUrlSearchParams("a&b=c");

var_dump($params->toList());
var_dump($params->toRfc1866String());
var_dump($params->toRfc3986String());
var_dump($params->toWhatWgUrlString());

?>
--EXPECT--
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    string(0) ""
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "b"
    [1]=>
    string(1) "c"
  }
}
string(6) "a=&b=c"
string(6) "a=&b=c"
string(6) "a=&b=c"
