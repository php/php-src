--TEST--
SPL: SplHeap recursive var_dump
--FILE--
<?php
$a = new SplMaxHeap;
$a->insert($a);
var_dump($a)
?>
===DONE===
--EXPECTF--
object(SplMaxHeap)#%d (3) {
  ["flags":"SplHeap":private]=>
  int(0)
  ["isCorrupted":"SplHeap":private]=>
  bool(false)
  ["heap":"SplHeap":private]=>
  array(1) {
    [0]=>
    *RECURSION*
  }
}
===DONE===
