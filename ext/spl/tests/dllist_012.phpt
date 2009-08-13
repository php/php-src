--TEST--
SPL: DoublyLinkedList: recursive var_dump
--FILE--
<?php
$a = new SplDoublyLinkedList;
$a[] = $a;

var_dump($a);
?>
===DONE===
--EXPECT--
object(SplDoublyLinkedList)#1 (2) {
  [u"flags":u"SplDoublyLinkedList":private]=>
  int(0)
  [u"dllist":u"SplDoublyLinkedList":private]=>
  array(1) {
    [0]=>
    object(SplDoublyLinkedList)#1 (2) {
      [u"flags":u"SplDoublyLinkedList":private]=>
      int(0)
      [u"dllist":u"SplDoublyLinkedList":private]=>
      array(1) {
        [0]=>
        *RECURSION*
      }
    }
  }
}
===DONE===
