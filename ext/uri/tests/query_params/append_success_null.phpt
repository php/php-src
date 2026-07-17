--TEST--
Test Uri\QueryParams::append() - success - null value
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("abc=123&foo=bar");
$params->append("baz", null);

var_dump($params->list());
var_dump($params->toRfc3986String());

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
    string(3) "baz"
    [1]=>
    NULL
  }
}
string(19) "abc=123&foo=bar&baz"
