--TEST--
array_chunk() - variation 22
--FILE--
<?php
$array = array ("b" => 1, "c" => 2);
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
array(2) {
  ["b"]=>
  int(1)
  ["c"]=>
  int(2)
}
[1]
array(2) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
  [1]=>
  array(1) {
    [0]=>
    int(2)
  }
}
array(2) {
  [0]=>
  array(1) {
    ["b"]=>
    int(1)
  }
  [1]=>
  array(1) {
    ["c"]=>
    int(2)
  }
}
array(2) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
  [1]=>
  array(1) {
    [0]=>
    int(2)
  }
}

[2]
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}
array(1) {
  [0]=>
  array(2) {
    ["b"]=>
    int(1)
    ["c"]=>
    int(2)
  }
}
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}
