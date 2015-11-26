--TEST--
Check that SplQueue can't be set to LIFO
--CREDITS--
Rob Knight <themanhimself@robknight.org.uk> PHPNW Test Fest 2009
--FILE--
<?php
$queue = new SplQueue();
try {
  $queue->setIteratorMode(SplDoublyLinkedList::IT_MODE_LIFO);
} catch (Exception $e) {
  echo $e->getMessage();
}
?>
--EXPECTF--
Iterators' LIFO/FIFO modes for SplStack/SplQueue objects are frozen
