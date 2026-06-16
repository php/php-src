--TEST--
Test Uri\QueryParams::deleteValue() - success - existing key, missing value
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("abc=123&abc=456&abc");
$params->deleteValue("abc", "");

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
    string(3) "abc"
    [1]=>
    string(3) "456"
  }
  [2]=>
  array(2) {
    [0]=>
    string(3) "abc"
    [1]=>
    NULL
  }
}
string(19) "abc=123&abc=456&abc"
