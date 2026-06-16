--TEST--
Test Uri\QueryParams::parseRfc3986() - success - basic
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("abc=123&foo=bar");

var_dump($params->list());
var_dump($params->toRfc3986String());

?>
--EXPECT--
array(2) {
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
}
string(15) "abc=123&foo=bar"
