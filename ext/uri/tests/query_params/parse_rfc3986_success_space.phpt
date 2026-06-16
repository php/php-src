--TEST--
Test Uri\QueryParams::parseRfc3986() - success - space character
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("a c=123&foo=b r");

var_dump($params->list());
var_dump($params->toRfc3986String());

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
string(19) "a%20c=123&foo=b%20r"
