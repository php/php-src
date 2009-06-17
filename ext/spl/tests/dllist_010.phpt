--TEST--
SPL: DoublyLinkedList: prev
--FILE--
<?php
$dll = new SplDoublyLinkedList();
$dll->push(1);
$dll->push(2);
$dll->push(3);
$dll->push(4);


$dll->rewind();
$dll->prev();
var_dump($dll->current());
$dll->rewind();
var_dump($dll->current());
$dll->next();
var_dump($dll->current());
$dll->next();
$dll->next();
var_dump($dll->current());
$dll->prev();
var_dump($dll->current());

?>
===DONE===
--EXPECT--
NULL
int(1)
int(2)
int(4)
int(3)
===DONE===
