--TEST--
Test Uri\QueryParams::parseRfc3986() - success - invalid percent-encoded UTF-8
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("a%FFz=b%FFy");

var_dump($params->toList());
var_dump($params->toRfc1866String());
var_dump($params->toRfc3986String());
var_dump($params->toWhatWgUrlString());

?>
--EXPECTF--
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(3) "%r\x61\xFF\x7A%r"
    [1]=>
    string(3) "%r\x62\xFF\x79%r"
  }
}
string(11) "a%FFz=b%FFy"
string(11) "a%FFz=b%FFy"
string(23) "a%EF%BF%BDz=b%EF%BF%BDy"
