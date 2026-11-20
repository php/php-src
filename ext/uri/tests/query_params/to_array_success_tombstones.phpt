--TEST--
Test Uri\QueryParams::toArray() - success - deleted entries do not cause conflicts or appear in arrays
--FILE--
<?php

$params = new Uri\QueryParams();
$params = $params->append("k", "1")->append("k", "2")->append("k[a]", "3")->delete("k");

var_dump($params->toArray());

?>
--EXPECT--
array(1) {
  ["k"]=>
  array(1) {
    ["a"]=>
    string(1) "3"
  }
}
