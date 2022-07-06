--TEST--
Array must be separated if unpacking by reference
--FILE--
<?php

function inc(&... $args) {
    foreach ($args as &$arg) {
        $arg++;
    }
}

$arr = [1, 2];
$arr[] = 3;
$arr2 = $arr;
inc(...$arr);
var_dump($arr);
var_dump($arr2);

?>
--EXPECT--
array(3) {
  [0]=>
  int(2)
  [1]=>
  int(3)
  [2]=>
  int(4)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
