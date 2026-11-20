--TEST--
Test Uri\QueryParams::parseWhatWgUrlSearchParams() - success - invalid percent-encoded UTF-8
--FILE--
<?php

$params = Uri\QueryParams::parseWhatWgUrlSearchParams("a%FFz=b%FFy");

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
    string(5) "a�z"
    [1]=>
    string(5) "b�y"
  }
}
string(23) "a%EF%BF%BDz=b%EF%BF%BDy"
string(23) "a%EF%BF%BDz=b%EF%BF%BDy"
string(23) "a%EF%BF%BDz=b%EF%BF%BDy"
