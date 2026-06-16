--TEST--
Test Uri\QueryParams::toArray() - success - one key, using the bracket syntax
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("abc[]=123");

var_dump($params->toArray());

?>
--EXPECT--
array(1) {
  ["abc"]=>
  array(1) {
    [0]=>
    string(3) "123"
  }
}
