--TEST--
Test Uri\QueryParams::append() - success - valid two-, three-, and four-byte UTF-8 sequences
--FILE--
<?php

$params = new Uri\QueryParams();
$params = $params->append("aé€😀z", "bé€😀y");

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
    string(11) "aé€😀z"
    [1]=>
    string(11) "bé€😀y"
  }
}
string(59) "a%C3%A9%E2%82%AC%F0%9F%98%80z=b%C3%A9%E2%82%AC%F0%9F%98%80y"
string(59) "a%C3%A9%E2%82%AC%F0%9F%98%80z=b%C3%A9%E2%82%AC%F0%9F%98%80y"
string(59) "a%C3%A9%E2%82%AC%F0%9F%98%80z=b%C3%A9%E2%82%AC%F0%9F%98%80y"
