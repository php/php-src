--TEST--
SplDoublyLinkedList offsetSet throws error on no parameters
--CREDITS--
PHPNW TestFest 2009 - Ben Longden
--FILE--
<?php
$list = new SplDoublyLinkedList();
$a = $list->offsetSet();
if(is_null($a)) {
	echo 'PASS';
}
?>
--EXPECTF--
Warning: SplDoublyLinkedList::offsetSet() expects exactly 2 parameters, 0 given in %s on line %d
PASS
