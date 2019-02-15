--TEST--
SPL: SplHeap check no arguments to be accepted on recoverFromCorruption
--CREDITS--
Rohan Abraham (rohanabrahams@gmail.com)
TestFest London May 2009
--FILE--
<?php
  $h = new SplMaxHeap();
  //Line below should throw a warning as no args are expected
  $h->recoverFromCorruption("no args");
?>
--EXPECTF--
Warning: SplHeap::recoverFromCorruption() expects exactly 0 parameters, 1 given in %s on line %d
