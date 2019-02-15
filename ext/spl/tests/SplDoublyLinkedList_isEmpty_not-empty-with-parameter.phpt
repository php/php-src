--TEST--
Check that SplDoublyLinkedList->isEmpty() returns an error message when a parameter is passed.
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
	//var_dump($dll);

	var_dump($dll->isEmpty("test"));
?>
--EXPECTF--
Warning: SplDoublyLinkedList::isEmpty() expects exactly 0 parameters, %d given in %s
NULL
