--TEST--
Test Uri\QueryParams::parseRfc3986() - success - percent-encoded space characters
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("a%20c=123&foo=b%20r");

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
    string(3) "123"
  }
  [1]=>
  array(2) {
    [0]=>
    string(3) "foo"
    [1]=>
    string(3) "b r"
  }
}
string(15) "a+c=123&foo=b+r"
string(19) "a%20c=123&foo=b%20r"
string(15) "a+c=123&foo=b+r"
