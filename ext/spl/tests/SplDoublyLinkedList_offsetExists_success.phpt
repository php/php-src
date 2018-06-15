--TEST--
SPL SplDoublyLinkedList offsetExists returns correct values
--CREDITS--
PHPNW TestFest 2009 - Ben Longden
--FILE--
<?php
$list = new SplDoublyLinkedList();

// Push two values onto the list
$list->push('abc');
$list->push('def');

// Validate that we can see the first value
if($list->offsetExists(0) === true) {
	echo "PASS\n";
}

// Validate that we can see the second value
if($list->offsetExists(1) === true) {
	echo "PASS\n";
}

// Check that there is no third value
if($list->offsetExists(2) === false) {
	echo "PASS\n";
}
?>
--EXPECT--
PASS
PASS
PASS
