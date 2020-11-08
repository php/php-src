--TEST--
array_chunk() - variation 24
--FILE--
<?php
$array = array ("a" => "A");
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
  ["a"]=>
  string(1) "A"
}
[1]
array(1) {
  [0]=>
  array(1) {
    [0]=>
    string(1) "A"
  }
}
array(1) {
  [0]=>
  array(1) {
    ["a"]=>
    string(1) "A"
  }
}
array(1) {
  [0]=>
  array(1) {
    [0]=>
    string(1) "A"
  }
}
