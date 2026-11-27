--TEST--
Test Uri\QueryParams::parseWhatWgUrlSearchParams() - success - a percent-encoded leading question mark is preserved
--FILE--
<?php

$params = Uri\QueryParams::parseWhatWgUrlSearchParams("%3Ffoo=bar");

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
    string(4) "?foo"
    [1]=>
    string(3) "bar"
  }
}
string(10) "%3Ffoo=bar"
string(8) "?foo=bar"
string(10) "%3Ffoo=bar"
