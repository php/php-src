--TEST--
Check that SplDoublyLinkedList->isEmpty() correctly returns true for an empty list.
--CREDITS--
PHPNW Testfest 2009 - Paul Court ( g@rgoyle.com )
--FILE--
<?php
	// Create a new Doubly Linked List
	$dll = new SplDoublyLinkedList();

	var_dump($dll->isEmpty());
?>
--EXPECT--
bool(true)
