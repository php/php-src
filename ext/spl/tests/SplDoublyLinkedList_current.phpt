--TEST--
SplDoublyLinkedList getIteratorMode
--CREDITS--
PHPNW Testfest 2009 - Lorna Mitchell
--FILE--
<?php
$list = new SplDoublyLinkedList();
var_dump($list->current());
?>
--EXPECT--
NULL
