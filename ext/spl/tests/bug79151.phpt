--TEST--
Bug #79151: heap use after free caused by spl_dllist_it_helper_move_forward
--FILE--
<?php

$a = new SplDoublyLinkedList();
$a->setIteratorMode(SplDoublyLinkedList::IT_MODE_LIFO | SplDoublyLinkedList::IT_MODE_DELETE);
$a->push(1);
$a->rewind();
$a->unshift(2);
var_dump($a->pop());
var_dump($a->next());

$a = new SplDoublyLinkedList();
$a->setIteratorMode(SplDoublyLinkedList::IT_MODE_FIFO | SplDoublyLinkedList::IT_MODE_DELETE);
$a->unshift(1);
$a->rewind();
$a->push(2);
var_dump($a->shift());
var_dump($a->next());

?>
--EXPECT--
int(1)
NULL
int(1)
NULL
