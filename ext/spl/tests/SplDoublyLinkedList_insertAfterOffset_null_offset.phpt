--TEST--
Check that SplDoublyLinkedList::insertAfterOffset throws an exception with an invalid offset argument
--FILE--
<?php
try {
	$dll = new SplDoublyLinkedList();
	var_dump($dll->insertAfterOffset(NULL,2));
} catch (OutOfRangeException $e) {
	echo "Exception: ".$e->getMessage()."\n";
}
?>
--EXPECTF--
Exception: Offset invalid or out of range
