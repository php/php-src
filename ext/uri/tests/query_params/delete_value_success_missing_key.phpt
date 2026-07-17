--TEST--
Test Uri\QueryParams::deleteValue() - success - missing key
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("abc=123&abc=456&abc=789");
$params->deleteValue("def", 456);

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
    string(3) "789"
  }
}
string(23) "abc=123&abc=456&abc=789"
