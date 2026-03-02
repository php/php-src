--TEST--
GH-16993: filter_var_array should emit warning for unknown filters
--EXTENSIONS--
filter
--FILE--
<?php
$data = ['test' => '42'];

$result = filter_var_array($data, ['test' => FILTER_VALIDATE_INT|FILTER_NULL_ON_FAILURE]);
var_dump($result);

$result = filter_var_array($data, ['test' => ['filter' => FILTER_VALIDATE_INT, 'flags' => FILTER_NULL_ON_FAILURE]]);
var_dump($result);
?>
--EXPECTF--
Warning: filter_var_array(): Unknown filter with ID 134217985 in %s on line %d
array(1) {
  ["test"]=>
  string(2) "42"
}
array(1) {
  ["test"]=>
  int(42)
}
