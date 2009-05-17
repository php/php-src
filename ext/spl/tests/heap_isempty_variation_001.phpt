--TEST--
SPL: SplHeap: isEmpty argument variation.
--FILE--
<?php
class SplHeap2 extends SplHeap{

  public function compare() {
           return -parent::compare();
       }
}

$h = new SplHeap2;
$h->isEmpty(1);
?>
--EXPECTF--
Warning: SplHeap::isEmpty() expects exactly 0 parameters, 1 given in %s
