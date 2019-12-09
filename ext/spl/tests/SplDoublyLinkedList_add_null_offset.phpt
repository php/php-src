--TEST--
Check that SplDoublyLinkedList::add throws an exception with an invalid offset argument
--FILE--
<?php
try {
	$dll = new SplDoublyLinkedList();
	var_dump($dll->add(NULL,2));
} catch (\ValueError $e) {
	echo $e->getMessage()."\n";
}
?>
--EXPECT--
SplDoublyLinkedList::add(): Argument #1 ($index) is out of range
