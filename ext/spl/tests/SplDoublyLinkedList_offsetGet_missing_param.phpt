--TEST--
Tests that the offsetGet() method throws an error when no argument is sent
--CREDITS--
PHPNW Test Fest 2009 - Rick Ogden
--FILE--
<?php
$dll = new SplDoublyLinkedList();
$dll->push(1);
$dll->push(2);

var_dump($dll->offsetGet());
?>
--EXPECTF--
Warning: SplDoublyLinkedList::offsetGet() expects exactly 1 parameter, 0 given in %s on line %d
NULL
