--TEST--
Checks that the shift() method of DoublyLinkedList does not accept args.
--CREDITS--
PHPNW Test Fest 2009 - Rick Ogden
--FILE--
<?php
$ll = new SplDoublyLinkedList();
$ll->push(1);
$ll->push(2);

var_dump($ll->shift(1));
?>
--EXPECTF--
Warning: SplDoublyLinkedList::shift() expects exactly 0 parameters, 1 given in %s on line %d
NULL
