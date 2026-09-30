--TEST--
Test Uri\QueryParams::toArray() - success - empty root and bracket keys
--FILE--
<?php

$params = new Uri\QueryParams();
$params = $params->append("[a]", "1")->append("[]", "2");

var_dump($params->toArray());

?>
--EXPECT--
array(1) {
  [""]=>
  array(2) {
    ["a"]=>
    string(1) "1"
    [0]=>
    string(1) "2"
  }
}
