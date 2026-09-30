--TEST--
Test Uri\QueryParams::parseRfc3986() - success - plus signs without percent-encoded bytes
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("a+b=c+d");

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
    string(3) "a+b"
    [1]=>
    string(3) "c+d"
  }
}
string(11) "a%2Bb=c%2Bd"
string(7) "a+b=c+d"
string(11) "a%2Bb=c%2Bd"
