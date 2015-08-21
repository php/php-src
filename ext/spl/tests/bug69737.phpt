--TEST--
Bug #69737 (Segfault when SplMinHeap::compare produces fatal error)
--FILE--
<?php
class SplMinHeap1 extends SplMinHeap {
  public function compare($a, $b) {
    return -parent::notexist($a, $b);
  }
}
$h = new SplMinHeap1();
$h->insert(1);
$h->insert(6);
?>
===DONE===
--EXPECTF--
Fatal error: Call to undefined method SplMinHeap::notexist() in %s%ebug69737.php on line %d
