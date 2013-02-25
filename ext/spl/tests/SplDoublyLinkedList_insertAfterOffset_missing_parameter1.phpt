--TEST--
Check that SplDoublyLinkedList::insertAfterOffset generate a warning and returns a NULL with missing arguments
--FILE--
<?php
$dll = new SplDoublyLinkedList();
var_dump($dll->insertAfterOffset());
?>
--EXPECTF--
Warning: SplDoublyLinkedList::insertAfterOffset() expects exactly 2 parameters, 0 given in %s on line %d
NULL

