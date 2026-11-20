--TEST--
Test Uri\QueryParams::append() - success - incomplete UTF-8 sequence
--FILE--
<?php

$params = new Uri\QueryParams();
$params = $params->append("a\xE2\x82", "a\xE2\x82");

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
    string(3) "a%r\xE2\x82%r"
    [1]=>
    string(3) "a%r\xE2\x82%r"
  }
}
string(15) "a%E2%82=a%E2%82"
string(15) "a%E2%82=a%E2%82"
string(21) "a%EF%BF%BD=a%EF%BF%BD"
