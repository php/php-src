--TEST--
SplDoublyLinkedList offsetSet throws error only one parameter
--CREDITS--
PHPNW TestFest 2009 - Ben Longden
--FILE--
<?php
$list = new SplDoublyLinkedList();
$a = $list->offsetSet(2);
if(is_null($a)) {
	echo 'PASS';
}
?>
--EXPECTF--
Warning: SplDoublyLinkedList::offsetSet() expects exactly 2 parameters, 1 given in %s on line %d
PASS
