--TEST--
Test Uri\QueryParams::append() - success - bool value
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("abc=123&foo=bar");
$params->append("baz", false);
$params->append("qux", true);

var_dump($params->list());
var_dump($params->toRfc3986String());

?>
--EXPECT--
array(4) {
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
    string(1) "0"
  }
  [3]=>
  array(2) {
    [0]=>
    string(3) "qux"
    [1]=>
    string(1) "1"
  }
}
string(27) "abc=123&foo=bar&baz=0&qux=1"
