--TEST--
Test Uri\QueryParams::parseRfc3986() - success - empty query param pair
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("a=");

var_dump($params->list());
var_dump($params->toRfc3986String());

?>
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    string(0) ""
  }
}
string(2) "a="
