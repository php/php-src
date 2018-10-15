--TEST--
Create a SplDoublyLinkedList, call setIteratorMode() and pass a SplDoublyLinkedList object as the parameter.
--CREDITS--
Philip Norton philipnorton42@gmail.com
--FILE--
<?php
$dll = new SplDoublyLinkedList(2);
$dll->setIteratorMode(new SplDoublyLinkedList(2));
?>
--EXPECTF--
Warning: SplDoublyLinkedList::setIteratorMode() expects parameter 1 to be int, object given in %s on line %d
