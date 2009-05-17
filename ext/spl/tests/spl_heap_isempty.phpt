--TEST--
SPL: Test of isEmpty for SPL Max Heap
--CREDITS--
Rohan Abraham (rohanabrahams@gmail.com)
TestFest London May 2009
--FILE--
<?php
  $h = new SplMaxHeap();
  echo "Checking a new heap is empty: ";
  var_dump($h->isEmpty())."\n";
  $h->insert(2);
  echo "Checking after insert: ";
  var_dump($h->isEmpty())."\n";
  $h->extract();
  echo "Checking after extract: ";
  var_dump($h->isEmpty())."\n";
?>
--EXPECT--
Checking a new heap is empty: bool(true)
Checking after insert: bool(false)
Checking after extract: bool(true)