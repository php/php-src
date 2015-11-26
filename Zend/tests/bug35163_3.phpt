--TEST--
Bug #35163.3 (Array elements can lose references)
--FILE--
<?php
$a = new stdClass;
$a->b = array(1);
$a->b[] =& $a->b;
$a->b[] =& $a->b;
$a->b[0] = 2;
var_dump($a);
$a->b = null;
$a = null;
?>
--EXPECTF--
object(stdClass)#%d (1) {
  ["b"]=>
  &array(3) {
    [0]=>
    int(2)
    [1]=>
    *RECURSION*
    [2]=>
    *RECURSION*
  }
}
