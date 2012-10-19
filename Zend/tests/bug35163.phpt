--TEST--
Bug #35163 (Array elements can lose references)
--FILE--
<?php
$a = array(array(1));
$a[0][] =& $a[0];
$a[0][] =& $a[0];
$a[0][0] = 2;
var_dump($a);
$a[0] = null;
$a = null;
?>
--EXPECT--
array(1) {
  [0]=>
  &array(3) {
    [0]=>
    int(2)
    [1]=>
    *RECURSION*
    [2]=>
    *RECURSION*
  }
}
