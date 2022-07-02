--TEST--
Iteration over SplDoublyLinkedList via 'foreach' does not change direction partway
--FILE--
<?php

$list = new SplDoublyLinkedList();
$list->push(1);
$list->push(2);
$list->push(3);

/* SplDoublyLinkedList would previously check the iteration mode *each time*
   it would advance to the next item in a 'foreach' loop
   This meant that it could move forward, then backward, then forward if the
   iteration mode was changed in the middle of a loop */

$list->setIteratorMode(SplDoublyLinkedList::IT_MODE_FIFO);
foreach ($list as $item) {
  $list->setIteratorMode(SplDoublyLinkedList::IT_MODE_LIFO);
  echo $item, "\n";
}

echo "***\n";

$list->setIteratorMode(SplDoublyLinkedList::IT_MODE_LIFO);
foreach ($list as $item) {
  $list->setIteratorMode(SplDoublyLinkedList::IT_MODE_FIFO);
  echo $item, "\n";
}


?>
--EXPECT--
1
2
3
***
3
2
1
