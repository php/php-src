--TEST--
Test Uri\QueryParams::toArray() - success - null values are preserved in scalars and arrays
--FILE--
<?php

$params = new Uri\QueryParams();
$params = $params->append("scalar", null)->append("list", null)->append("list", "value")->append("nested[a]", null)->append("brackets[]", null);

var_dump($params->toArray());

?>
--EXPECT--
array(4) {
  ["scalar"]=>
  NULL
  ["list"]=>
  array(2) {
    [0]=>
    NULL
    [1]=>
    string(5) "value"
  }
  ["nested"]=>
  array(1) {
    ["a"]=>
    NULL
  }
  ["brackets"]=>
  array(1) {
    [0]=>
    NULL
  }
}
