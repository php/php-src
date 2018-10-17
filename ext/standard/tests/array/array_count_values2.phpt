--TEST--
basic array_count_values test
--FILE--
<?php
$array1 = array(1,
				"hello",
				1,
				"world",
				"hello",
				"",
				"rabbit",
				"foo",
				"Foo",
				TRUE,
				FALSE,
				NULL,
				0);
var_dump(array_count_values($array1));
?>
--EXPECTF--
Warning: array_count_values(): Can only count STRING and INTEGER values! in %s on line %d

Warning: array_count_values(): Can only count STRING and INTEGER values! in %s on line %d

Warning: array_count_values(): Can only count STRING and INTEGER values! in %s on line %d
array(8) {
  [1]=>
  int(2)
  ["hello"]=>
  int(2)
  ["world"]=>
  int(1)
  [""]=>
  int(1)
  ["rabbit"]=>
  int(1)
  ["foo"]=>
  int(1)
  ["Foo"]=>
  int(1)
  [0]=>
  int(1)
}
