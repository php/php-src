--TEST--
Test Uri\QueryParams::toArray() - success - multiple keys, using brackets
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("abc[]=123&abc[]=456");

var_dump($params->toArray());

?>
--EXPECT--
array(1) {
  ["abc"]=>
  array(2) {
    [0]=>
    string(3) "123"
    [1]=>
    string(3) "456"
  }
}
