--TEST--
SplDoublyLinkedList::top() - pass in an unexpected null parameter
--CREDITS--
PHPNW Testfest 2009 - Adrian Hardy
--FILE--
<?php

$list = new SplDoublyLinkedList();
$list->push("top");
$list->top(null);

?>
--EXPECTF--
Warning: SplDoublyLinkedList::top() expects exactly 0 parameters, 1 given in %s on line %d
