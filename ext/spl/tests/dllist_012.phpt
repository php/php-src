--TEST--
SPL: DoublyLinkedList: recursive var_dump
--FILE--
<?php
$a = new SplDoublyLinkedList;
$a[] = $a;

var_dump($a);
?>
===DONE===
--EXPECTF--
object(SplDoublyLinkedList)#%d (2) {
  ["flags":"SplDoublyLinkedList":private]=>
  int(0)
  ["dllist":"SplDoublyLinkedList":private]=>
  array(1) {
    [0]=>
    *RECURSION*
  }
}
===DONE===
