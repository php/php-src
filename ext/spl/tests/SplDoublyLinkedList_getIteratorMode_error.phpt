--TEST--
SplDoublyLinkedList getIteratorMode with an unexpected parameter
--CREDITS--
PHPNW Testfest 2009 - Lorna Mitchell
--FILE--
<?php
$list = new SplDoublyLinkedList();
$list->getIteratorMode(24);
?>
--EXPECTF--
Warning: SplDoublyLinkedList::getIteratorMode() expects exactly 0 parameters, 1 given in %s on line %d

