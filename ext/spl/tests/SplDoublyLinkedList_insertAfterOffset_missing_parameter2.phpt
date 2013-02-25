--TEST--
Check that SplDoublyLinkedList::insertAfterOffset generate a warning and returns a NULL with a missing value argument
--FILE--
<?php
$dll = new SplDoublyLinkedList();
var_dump($dll->insertAfterOffset(2));
?>
--EXPECTF--
Warning: SplDoublyLinkedList::insertAfterOffset() expects exactly 2 parameters, 1 given in %s on line %d
NULL

