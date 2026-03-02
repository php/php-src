--TEST--
SplDoublyLinkedList::offsetGet() with 1st parameter passed as array.
--CREDITS--
PHPNW Test Fest 2009 - Jordan Hatch
--FILE--
<?php

$array = new SplDoublyLinkedList( );

$get = $array->offsetGet( array( 'fail' ) );

?>
--EXPECTF--
Fatal error: Uncaught TypeError: SplDoublyLinkedList::offsetGet(): Argument #1 ($index) must be of type int, array given in %s
Stack trace:
#0 %s
#1 {main}
  thrown in %s on line %d
