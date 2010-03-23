--TEST--
SPL: FixedArray: var_dump
--FILE--
<?php
$a = new SplFixedArray(2);
$a[0] = "foo";
var_dump(empty($a[0]), empty($a[1]), $a);
?>
--EXPECTF--
bool(false)
bool(true)
object(SplFixedArray)#%d (2) {
  [0]=>
  string(3) "foo"
  [1]=>
  NULL
}
