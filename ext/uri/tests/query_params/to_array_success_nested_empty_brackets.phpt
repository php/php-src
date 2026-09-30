--TEST--
Test Uri\QueryParams::toArray() - success - empty brackets create a new nested array for each occurrence
--FILE--
<?php

$params = new Uri\QueryParams();
$params = $params->append("k[][a]", "1")->append("k[][b]", "2");

var_dump($params->toArray());

?>
--EXPECT--
array(1) {
  ["k"]=>
  array(2) {
    [0]=>
    array(1) {
      ["a"]=>
      string(1) "1"
    }
    [1]=>
    array(1) {
      ["b"]=>
      string(1) "2"
    }
  }
}
