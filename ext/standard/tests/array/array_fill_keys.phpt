--TEST--
basic array_fill_keys test
--INI--
precision=14
--FILE--
<?php
	var_dump(array_fill_keys('test', 1));
	var_dump(array_fill_keys(array(), 1));
	var_dump(array_fill_keys(array('foo', 'bar'), NULL));
	var_dump(array_fill_keys(array('5', 'foo', 10, 1.23), 123));
	var_dump(array_fill_keys(array('test', TRUE, 10, 100), ''));
?>
--EXPECTF--

Warning: array_fill_keys() expects parameter 1 to be array, string given in %s on line %d
NULL
array(0) {
}
array(2) {
  ["foo"]=>
  NULL
  ["bar"]=>
  NULL
}
array(4) {
  [5]=>
  int(123)
  ["foo"]=>
  int(123)
  [10]=>
  int(123)
  ["1.23"]=>
  int(123)
}
array(4) {
  ["test"]=>
  string(0) ""
  [1]=>
  string(0) ""
  [10]=>
  string(0) ""
  [100]=>
  string(0) ""
}
