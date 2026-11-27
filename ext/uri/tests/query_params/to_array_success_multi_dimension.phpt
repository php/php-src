--TEST--
Test Uri\QueryParams::toArray() - success - multi-dimension array
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("foo%5Bbar%5D%5Bbaz%5D=123&foo%5Bqux%5D=quux");

var_dump($params->toArray());

?>
--EXPECT--
array(1) {
  ["foo"]=>
  array(2) {
    ["bar"]=>
    array(1) {
      ["baz"]=>
      string(3) "123"
    }
    ["qux"]=>
    string(4) "quux"
  }
}
