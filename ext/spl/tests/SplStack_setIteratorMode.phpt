--TEST--
Check that SplStack can't be set to FIFO
--CREDITS--
Rob Knight <themanhimself@robknight.org.uk> PHPNW Test Fest 2009
--FILE--
<?php
$stack = new SplStack();
try {
  $stack->setIteratorMode(SplDoublyLinkedList::IT_MODE_FIFO);
} catch (Exception $e) {
  echo $e->getMessage();
}
?>
--EXPECT--
Iterators' LIFO/FIFO modes for SplStack/SplQueue objects are frozen
