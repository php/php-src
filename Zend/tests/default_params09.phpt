--TEST--
Default parameters - 9, internal funcs
--FILE--
<?php
var_dump(array_chunk([1,2,3,4], 2));
var_dump(array_chunk([1,2,3,4], 2, default));
echo "Done\n";
?>
--EXPECTF--	
array(2) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
  [1]=>
  array(2) {
    [0]=>
    int(3)
    [1]=>
    int(4)
  }
}
array(2) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
  [1]=>
  array(2) {
    [0]=>
    int(3)
    [1]=>
    int(4)
  }
}
Done