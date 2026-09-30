--TEST--
Test Uri\QueryParams::toArray() - success - zero allows a flat array and literal malformed names
--FILE--
<?php

$params = new Uri\QueryParams();
$params = $params->append("a", "1")->append("k[a][", "2");

var_dump($params->toArray(0));

?>
--EXPECT--
array(2) {
  ["a"]=>
  string(1) "1"
  ["k[a]["]=>
  string(1) "2"
}
