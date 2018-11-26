--TEST--
SPL: SplHeap: isEmpty argument variation.
--FILE--
<?php

class SplHeap2 extends SplHeap {
  public function compare($a, $b) {
    return -parent::compare($a, $b);
  }
}

$h = new SplHeap2();
$h->isEmpty(1);
?>
--EXPECTF--
Warning: SplHeap::isEmpty() expects exactly 0 parameters, 1 given in %s
