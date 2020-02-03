--TEST--
array_chunk() - variation 26
--FILE--
<?php
$array =array (0);
var_dump ($array);
for ($i = 1; $i < (sizeof($array) + 1); $i++) {
    echo "[$i]\n";
    var_dump (array_chunk ($array, $i));
    var_dump (array_chunk ($array, $i, TRUE));
    var_dump (array_chunk ($array, $i, FALSE));
    echo "\n";
}
?>
--EXPECT--
array(1) {
  [0]=>
  int(0)
}
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
    [0]=>
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
