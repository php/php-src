--TEST--
Doubly Linked List - offsetUnset > number elements

--CREDITS--
PHPNW Test Fest 2009 - Mat Griffin

--FILE--
<?php
$ll = new SplDoublyLinkedList();

$ll->push('1');
$ll->push('2');
$ll->push('3');

try {

$ll->offsetUnset($ll->count() + 1);

var_dump($ll);

} catch(Exception $e) {
echo $e->getMessage();
}

?>
--EXPECT--
Offset out of range
