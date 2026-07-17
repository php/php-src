--TEST--
Test Uri\QueryParams::toArray() - success - one key, without brackets
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("abc=123");

var_dump($params->toArray());

?>
--EXPECT--
array(1) {
  ["abc"]=>
  string(3) "123"
}
