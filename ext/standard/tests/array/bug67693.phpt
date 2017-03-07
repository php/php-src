--TEST--
Bug #67693 - incorrect push to empty array
--FILE--
<?php

$array = array(-1 => 0);

array_pop($array);

array_push($array, 0);
array_push($array, 0);

var_dump($array);

echo"\nDone";
?>
--EXPECT--
array(2) {
  [-1]=>
  int(0)
  [0]=>
  int(0)
}

Done
