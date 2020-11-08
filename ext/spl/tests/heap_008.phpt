--TEST--
SPL: SplHeap: var_dump
--FILE--
<?php
$h = new SplMaxHeap();

$h->insert(1);
$h->insert(5);
$h->insert(0);
$h->insert(4);

var_dump($h);
?>
--EXPECT--
object(SplMaxHeap)#1 (3) {
  ["flags":"SplHeap":private]=>
  int(0)
  ["isCorrupted":"SplHeap":private]=>
  bool(false)
  ["heap":"SplHeap":private]=>
  array(4) {
    [0]=>
    int(5)
    [1]=>
    int(4)
    [2]=>
    int(0)
    [3]=>
    int(1)
  }
}
