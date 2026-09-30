--TEST--
Test Uri\QueryParams::toArray() - success - normal PHP append semantics after a negative index
--FILE--
<?php

$params = new Uri\QueryParams();
$params = $params->append("k[-5]", "a")->append("k[]", "b");

var_dump($params->toArray());

?>
--EXPECT--
array(1) {
  ["k"]=>
  array(2) {
    [-5]=>
    string(1) "a"
    [-4]=>
    string(1) "b"
  }
}
