--TEST--
SPL: SplHeap recursive var_dump
--FILE--
<?php
$a = new SplMaxHeap;
$a->insert($a);
var_dump($a)
?>
===DONE===
--EXPECT--
object(SplMaxHeap)#1 (3) {
  [u"flags":u"SplHeap":private]=>
  int(0)
  [u"isCorrupted":u"SplHeap":private]=>
  bool(false)
  [u"heap":u"SplHeap":private]=>
  array(1) {
    [0]=>
    object(SplMaxHeap)#1 (3) {
      [u"flags":u"SplHeap":private]=>
      int(0)
      [u"isCorrupted":u"SplHeap":private]=>
      bool(false)
      [u"heap":u"SplHeap":private]=>
      array(1) {
        [0]=>
        *RECURSION*
      }
    }
  }
}
===DONE===
