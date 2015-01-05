--TEST--
Check that SplDoublyLinkedList::push generate a warning and return NULL with missing param
--CREDITS--
PHPNW Testfest 2009 - Simon Westcott (swestcott@gmail.com)
--FILE--
<?php
$dll = new SplDoublyLinkedList();
var_dump($dll->push());
?>
--EXPECTF--
Warning: SplDoublyLinkedList::push() expects exactly 1 parameter, 0 given in %s on line %d
NULL

