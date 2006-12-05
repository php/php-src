--TEST--
Bug #33382 ( array_reverse() fails after *sort() )
--FILE--
<?php

$array = array(1,2,3,4,5);

sort($array);

var_dump(array_reverse($array));

echo "Done\n";
?>
--EXPECT--	
array(5) {
  [0]=>
  int(5)
  [1]=>
  int(4)
  [2]=>
  int(3)
  [3]=>
  int(2)
  [4]=>
  int(1)
}
Done
