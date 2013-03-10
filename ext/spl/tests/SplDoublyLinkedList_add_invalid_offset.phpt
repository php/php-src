--TEST--
Check that SplDoublyLinkedList::add throws an exception with an invalid offset argument
--FILE--
<?php
try {
	$dll = new SplDoublyLinkedList();
	var_dump($dll->add(12,'Offset 12 should not exist'));
} catch (OutOfRangeException $e) {
	echo "Exception: ".$e->getMessage()."\n";
}
?>
--EXPECTF--
Exception: Offset invalid or out of range
