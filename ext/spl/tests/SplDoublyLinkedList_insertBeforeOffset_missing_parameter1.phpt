--TEST--
Check that SplDoublyLinkedList::insertBeforeOffset generate a warning and returns a NULL with missing arguments
--FILE--
<?php
$dll = new SplDoublyLinkedList();
var_dump($dll->insertBeforeOffset());
?>
--EXPECTF--
Warning: SplDoublyLinkedList::insertBeforeOffset() expects exactly 2 parameters, 0 given in %s on line %d
NULL

