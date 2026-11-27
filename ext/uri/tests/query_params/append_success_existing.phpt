--TEST--
Test Uri\QueryParams::append() - success - existing parameter
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("abc=123&foo=bar");
$params = $params->append("foo", "bar");

var_dump($params->toList());
var_dump($params->toRfc1866String());
var_dump($params->toRfc3986String());
var_dump($params->toWhatWgUrlString());

?>
--EXPECT--
array(3) {
  [0]=>
  array(2) {
    [0]=>
    string(3) "abc"
    [1]=>
    string(3) "123"
  }
  [1]=>
  array(2) {
    [0]=>
    string(3) "foo"
    [1]=>
    string(3) "bar"
  }
  [2]=>
  array(2) {
    [0]=>
    string(3) "foo"
    [1]=>
    string(3) "bar"
  }
}
string(23) "abc=123&foo=bar&foo=bar"
string(23) "abc=123&foo=bar&foo=bar"
string(23) "abc=123&foo=bar&foo=bar"
