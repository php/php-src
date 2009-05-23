--TEST--
SplDoublyLinkedList::bottom() - pass in an unexpected array parameter
--CREDITS--
PHPNW Testfest 2009 - Adrian Hardy
--FILE--
<?php

$list = new SplDoublyLinkedList();
$list->push("top");
$list->bottom(array());

?>
--EXPECTF--
Warning: SplDoublyLinkedList::bottom() expects exactly 0 parameters, 1 given in %s on line %d
