--TEST--
array_count_values()
--FILE--
<?php
$arrays = array (
	array (),
	array (0),
	array (1),
	array (-1),
	array (0, 0),
	array (0, 1),
	array (1, 1),
	array (1, "hello", 1, "world", "hello"),
	array ("hello", "world", "hello"),
	array ("", "world", "", "hello", "world", "hello", "hello", "world", "hello"),
	array (0, array (1, "hello", 1, "world", "hello")),
	array (1, array (1, "hello", 1, "world", "hello"), array (1, "hello", 1, "world", "hello"), array (1, "hello", 1, "world", "hello")),
);

foreach ($arrays as $item) {
	var_dump (@array_count_values ($item));
	echo "\n";
}
?>
--EXPECT--
array(0) {
}

array(1) {
  [0]=>
  int(1)
}

array(1) {
  [1]=>
  int(1)
}

array(1) {
  [-1]=>
  int(1)
}

array(1) {
  [0]=>
  int(2)
}

array(2) {
  [0]=>
  int(1)
  [1]=>
  int(1)
}

array(1) {
  [1]=>
  int(2)
}

array(3) {
  [1]=>
  int(2)
  ["hello"]=>
  int(2)
  ["world"]=>
  int(1)
}

array(2) {
  ["hello"]=>
  int(2)
  ["world"]=>
  int(1)
}

array(3) {
  [""]=>
  int(2)
  ["world"]=>
  int(3)
  ["hello"]=>
  int(4)
}

array(1) {
  [0]=>
  int(1)
}

array(1) {
  [1]=>
  int(1)
}

