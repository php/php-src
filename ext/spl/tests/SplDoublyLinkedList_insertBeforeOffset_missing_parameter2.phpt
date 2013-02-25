--TEST--
Check that SplDoublyLinkedList::insertBeforeOffset generate a warning and returns a NULL with a missing value argument
--FILE--
<?php
$dll = new SplDoublyLinkedList();
var_dump($dll->insertBeforeOffset(2));
?>
--EXPECTF--
Warning: SplDoublyLinkedList::insertBeforeOffset() expects exactly 2 parameters, 1 given in %s on line %d
NULL

