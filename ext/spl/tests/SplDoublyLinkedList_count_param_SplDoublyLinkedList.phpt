--TEST--
Create a SplDoublyLinkedList, call count() and pass a SplDoublyLinkedList object as the parameter.
--CREDITS--
Philip Norton philipnorton42@gmail.com
--FILE--
<?php
$dll = new SplDoublyLinkedList(2);
$dll->count(new SplDoublyLinkedList(2));
?>
--EXPECTF--
Warning: SplDoublyLinkedList::count() expects exactly 0 parameters, 1 given in %s on line %d
