--TEST--
SplDoublyLinkedList::offsetGet() with no parameter passed.
--CREDITS--
PHPNW Test Fest 2009 - Jordan Hatch
--FILE--
<?php

$array = new SplDoublyLinkedList( );

$get = $array->pop( 'param' );

?>
--EXPECTF--
Warning: SplDoublyLinkedList::pop() expects exactly 0 parameters, 1 given in %s on line %d