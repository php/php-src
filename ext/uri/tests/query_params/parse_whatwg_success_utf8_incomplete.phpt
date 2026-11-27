--TEST--
Test Uri\QueryParams::parseWhatWgUrlSearchParams() - success - incomplete percent-encoded UTF-8
--FILE--
<?php

$params = Uri\QueryParams::parseWhatWgUrlSearchParams("a%E2%82=b%E2%82");

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
    string(4) "a�"
    [1]=>
    string(4) "b�"
  }
}
string(21) "a%EF%BF%BD=b%EF%BF%BD"
string(21) "a%EF%BF%BD=b%EF%BF%BD"
string(21) "a%EF%BF%BD=b%EF%BF%BD"
