--TEST--
Check that SplDoublyLinkedList::add generate a warning and returns a NULL with a missing value argument
--FILE--
<?php
$dll = new SplDoublyLinkedList();
var_dump($dll->add(2));
?>
--EXPECTF--
Warning: SplDoublyLinkedList::add() expects exactly 2 parameters, 1 given in %s on line %d
NULL
