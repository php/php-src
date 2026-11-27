--TEST--
Test Uri\QueryParams::toArray() - success - nesting depth exactly at the limit
--FILE--
<?php

$params = new Uri\QueryParams();
$params = $params->append("k[a][b]", "1");

var_dump($params->toArray(2));

?>
--EXPECT--
array(1) {
  ["k"]=>
  array(1) {
    ["a"]=>
    array(1) {
      ["b"]=>
      string(1) "1"
    }
  }
}
