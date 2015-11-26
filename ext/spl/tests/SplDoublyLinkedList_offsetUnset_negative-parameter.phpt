--TEST--
Check that SplDoublyLinkedList->offsetUnset() returns an error message when the offset is < 0.
--CREDITS--
PHPNW Testfest 2009 - Paul Court ( g@rgoyle.com )
--FILE--
<?php
	// Create a new Doubly Linked List
	$dll = new SplDoublyLinkedList();
	
	// Add some items to the list
	$dll->push(1);
	$dll->push(2);
	$dll->push(3);
	
	try {
		$dll->offsetUnset(-1);
	}
	catch (Exception $e) {
		echo $e->getMessage() . "\n";
	}
?>
--EXPECT--
Offset out of range
