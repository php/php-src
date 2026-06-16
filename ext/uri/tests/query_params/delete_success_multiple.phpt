--TEST--
Test Uri\QueryParams::delete() - success - multiple values
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("abc=123&abc=456&abc=789&foo=bar");
$params->delete("abc");

var_dump($params->list());
var_dump($params->toRfc3986String());

?>
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(3) "foo"
    [1]=>
    string(3) "bar"
  }
}
string(7) "foo=bar"
