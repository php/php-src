--TEST--
SplDoublyLinkedList::offsetGet() with no parameter passed.
--CREDITS--
PHPNW Test Fest 2009 - Jordan Hatch
--FILE--
<?php

$array = new SplDoublyLinkedList( );

$get = $array->offsetGet();

?>
--EXPECTF--
Warning: SplDoublyLinkedList::offsetGet() expects exactly 1 parameter, 0 given in %s on line %d