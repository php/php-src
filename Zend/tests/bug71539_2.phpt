--TEST--
Bug #71539.2 (Memory error on $arr[$a] =& $arr[$b] if RHS rehashes)
--FILE--
<?php
$a = [0,1,2,3,4,5,6];
$a[200] =& $a[100];
$a[100] =42;
var_dump($a);
?>
--EXPECT--
array(9) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(4)
  [5]=>
  int(5)
  [6]=>
  int(6)
  [100]=>
  &int(42)
  [200]=>
  &int(42)
}
