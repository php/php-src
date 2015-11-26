--TEST--
SPL: DoublyLinkedList: prev
--FILE--
<?php
$dll = new SplDoublyLinkedList();
$dll->rewind();
$dll->prev();
var_dump($dll->current());
?>
===DONE===
--EXPECT--
NULL
===DONE===
