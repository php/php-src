--TEST--
Test Uri\QueryParams::deleteValue() - success - existing key, existing value
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("abc=123&abc=456&abc=789");
$params->deleteValue("abc", 456);

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
    string(3) "abc"
    [1]=>
    string(3) "789"
  }
}
string(15) "abc=123&abc=789"
