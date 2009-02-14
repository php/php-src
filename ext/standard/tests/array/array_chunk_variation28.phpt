--TEST--
array_chunk() - variation 28
--FILE--
<?php
$array = array (-1);
var_dump ($array);
for ($i = 0; $i < (sizeof($array) + 1); $i++) {
	echo "[$i]\n";
	var_dump (@array_chunk ($array, $i));
	var_dump (@array_chunk ($array, $i, TRUE));
	var_dump (@array_chunk ($array, $i, FALSE));
	echo "\n";
}
?>
--EXPECT--
array(1) {
  [0]=>
  int(-1)
}
[0]
NULL
NULL
NULL

[1]
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(-1)
  }
}
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(-1)
  }
}
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(-1)
  }
}
