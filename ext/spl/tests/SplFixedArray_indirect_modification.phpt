--TEST--
SplFixedArray indirect modification notice
--FILE--
<?php
$a = new SplFixedArray(1);
$a[0][] = 3;
var_dump($a);
?>
--EXPECTF--
Notice: Indirect modification of overloaded element of SplFixedArray has no effect in %s on line %d
object(SplFixedArray)#1 (1) {
  [0]=>
  NULL
}
