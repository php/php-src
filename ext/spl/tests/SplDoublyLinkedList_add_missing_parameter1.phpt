--TEST--
Check that SplDoublyLinkedList::add generate a warning and returns a NULL with missing arguments
--FILE--
<?php
$dll = new SplDoublyLinkedList();
var_dump($dll->add());
?>
--EXPECTF--
Warning: SplDoublyLinkedList::add() expects exactly 2 parameters, 0 given in %s on line %d
NULL
