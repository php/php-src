--TEST--
array_chunk() - variation 31
--FILE--
<?php
$array = array (1 => 0);
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
  [1]=>
  int(0)
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
    int(0)
  }
}
array(1) {
  [0]=>
  array(1) {
    [1]=>
    int(0)
  }
}
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(0)
  }
}
