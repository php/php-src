--TEST--
SPL SplDoublyLinkedList offsetExists displays warning and returns null on no parameters
--CREDITS--
PHPNW TestFest 2009 - Ben Longden
--FILE--
<?php
$list = new SplDoublyLinkedList();
$a = $list->offsetExists();
if(is_null($a)) {
	echo 'PASS';
}
?>
--EXPECTF--
Warning: SplDoublyLinkedList::offsetExists() expects exactly 1 parameter, 0 given in %s on line %d
PASS
