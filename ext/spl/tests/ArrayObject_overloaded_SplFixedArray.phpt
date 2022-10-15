--TEST--
SplFixedArray properties is compatible with ArrayObject
--FILE--
<?php
$ao = new ArrayObject([1, 2, 3]);
$fixedArray = new SplFixedArray(1);
$fixedArray[0] = new SplDoublyLinkedList();
$ao->exchangeArray($fixedArray);
$ao[0] = new stdClass();
var_dump($ao);
?>
--EXPECT--
object(ArrayObject)#1 (1) {
  ["storage":"ArrayObject":private]=>
  object(SplFixedArray)#2 (2) {
    [0]=>
    object(SplDoublyLinkedList)#3 (2) {
      ["flags":"SplDoublyLinkedList":private]=>
      int(0)
      ["dllist":"SplDoublyLinkedList":private]=>
      array(0) {
      }
    }
    ["0"]=>
    object(stdClass)#4 (0) {
    }
  }
}
