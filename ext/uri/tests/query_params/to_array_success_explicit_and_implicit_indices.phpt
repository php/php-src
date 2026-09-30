--TEST--
Test Uri\QueryParams::toArray() - success - appending after an explicit index
--FILE--
<?php

$params = new Uri\QueryParams();
$params = $params->append("k[2]", "a")->append("k[]", "b");

var_dump($params->toArray());

?>
--EXPECT--
array(1) {
  ["k"]=>
  array(2) {
    [2]=>
    string(1) "a"
    [3]=>
    string(1) "b"
  }
}
