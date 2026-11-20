--TEST--
Test Uri\QueryParams::append() - success - invalid UTF-8 byte
--FILE--
<?php

$params = new Uri\QueryParams();
$params = $params->append("a\xFFz", "a\xFFz");

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
    string(3) "a%r\xFF%rz"
    [1]=>
    string(3) "a%r\xFF%rz"
  }
}
string(11) "a%FFz=a%FFz"
string(11) "a%FFz=a%FFz"
string(23) "a%EF%BF%BDz=a%EF%BF%BDz"
