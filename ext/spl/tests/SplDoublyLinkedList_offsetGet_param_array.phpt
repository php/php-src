--TEST--
SplDoublyLinkedList::offsetGet() with 1st parameter passed as array.
--CREDITS--
PHPNW Test Fest 2009 - Jordan Hatch
--FILE--
<?php

$array = new SplDoublyLinkedList( );

try {
    $array->offsetGet( array( 'fail' ) );
} catch(\ValueError $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
SplDoublyLinkedList::offsetGet(): Argument #1 ($index) is out of range
