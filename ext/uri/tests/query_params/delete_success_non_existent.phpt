--TEST--
Test Uri\QueryParams::delete() - success - non-existent key
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("abc=123&foo=bar");
$params->delete("xyz");

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
