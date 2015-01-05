--TEST--
Checks that the pop() method of DoublyLinkedList does not accept args.
--CREDITS--
PHPNW Test Fest 2009 - Rick Ogden
--FILE--
<?php
$ll = new SplDoublyLinkedList();
$ll->push(1);
$ll->push(2);

var_dump($ll->pop(1));
?>
--EXPECTF--
Warning: SplDoublyLinkedList::pop() expects exactly 0 parameters, 1 given in %s on line %d
NULL
