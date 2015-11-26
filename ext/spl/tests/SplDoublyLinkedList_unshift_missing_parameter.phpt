--TEST--
Check that SplDoublyLinkedList::unshift generate a warning and return NULL with missing param
--CREDITS--
PHPNW Testfest 2009 - Simon Westcott (swestcott@gmail.com)
--FILE--
<?php
$dll = new SplDoublyLinkedList();
var_dump($dll->unshift());
?>
--EXPECTF--
Warning: SplDoublyLinkedList::unshift() expects exactly 1 parameter, 0 given in %s on line %d
NULL

