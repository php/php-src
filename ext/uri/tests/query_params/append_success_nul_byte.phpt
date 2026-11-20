--TEST--
Test Uri\QueryParams::append() - success - contains NUL bytes
--FILE--
<?php

$params = new Uri\QueryParams();
$params = $params->append("a\0b", "c\0d");

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
    string(3) "a%0b"
    [1]=>
    string(3) "c%0d"
  }
}
string(11) "a%r\x25%r00b=c%r\x25%r00d"
string(11) "a%r\x25%r00b=c%r\x25%r00d"
string(11) "a%r\x25%r00b=c%r\x25%r00d"
