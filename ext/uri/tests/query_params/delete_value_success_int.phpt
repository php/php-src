--TEST--
Test Uri\QueryParams::deleteValue() - success - int value
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("abc=123&abc=456&abc=foo");
$params->deleteValue("abc", 123);

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
    string(3) "456"
  }
  [1]=>
  array(2) {
    [0]=>
    string(3) "abc"
    [1]=>
    string(3) "foo"
  }
}
string(15) "abc=456&abc=foo"
