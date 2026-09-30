--TEST--
Test Uri\QueryParams::append() - success - percent-encoding names and values
--FILE--
<?php

$params = new Uri\QueryParams();
$params = $params->append("a c", "#123")->append("<foo>", "b@r~*");

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
    string(3) "a c"
    [1]=>
    string(4) "#123"
  }
  [1]=>
  array(2) {
    [0]=>
    string(5) "<foo>"
    [1]=>
    string(5) "b@r~*"
  }
}
string(30) "a+c=%23123&%3Cfoo%3E=b%40r~%2A"
string(28) "a%20c=%23123&%3Cfoo%3E=b@r~*"
string(30) "a+c=%23123&%3Cfoo%3E=b%40r%7E*"
