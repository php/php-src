--TEST--
SplDoublyLinkedList getIteratorMode
--CREDITS--
PHPNW Testfest 2009 - Lorna Mitchell
--FILE--
<?php
$list = new SplDoublyLinkedList();
$list->setIteratorMode(SplDoublyLinkedList::IT_MODE_FIFO | SplDoublyLinkedList::IT_MODE_KEEP);
echo $list->getIteratorMode(), "\n";
$list->setIteratorMode(SplDoublyLinkedList::IT_MODE_LIFO);
echo $list->getIteratorMode(), "\n";
?>
--EXPECT--
0
2
