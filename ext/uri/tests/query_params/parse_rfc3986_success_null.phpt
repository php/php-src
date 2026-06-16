--TEST--
Test Uri\QueryParams::parseRfc3986() - success - null value
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("a&b=c");

var_dump($params->list());
var_dump($params->toRfc3986String());

?>
--EXPECT--
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    NULL
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "b"
    [1]=>
    string(1) "c"
  }
}
string(5) "a&b=c"
