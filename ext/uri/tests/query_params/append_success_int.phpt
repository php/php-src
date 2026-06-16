--TEST--
Test Uri\QueryParams::append() - success - integer value

--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("abc=123&foo=bar");
$params->append("baz", 0);

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
    string(1) "0"
  }
}
string(21) "abc=123&foo=bar&baz=0"
