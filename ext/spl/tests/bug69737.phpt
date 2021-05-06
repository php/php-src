--TEST--
Bug #69737 (Segfault when SplMinHeap::compare produces fatal error)
--FILE--
<?php
class SplMinHeap1 extends SplMinHeap {
  public function compare($a, $b): int {
    return -parent::notexist($a, $b);
  }
}
$h = new SplMinHeap1();
$h->insert(1);
$h->insert(6);
?>
===DONE===
--EXPECTF--
Fatal error: Uncaught Error: Call to undefined method SplMinHeap::notexist() in %s%ebug69737.php:%d
Stack trace:
#0 [internal function]: SplMinHeap1->compare(1, 6)
#1 %s%ebug69737.php(%d): SplHeap->insert(6)
#2 {main}
  thrown in %s%ebug69737.php on line %d
