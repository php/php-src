--TEST--
Bug #73629 (SplDoublyLinkedList::setIteratorMode masks intern flags)
--FILE--
<?php
$q = new SplQueue();
try {
    $q->setIteratorMode(SplDoublyLinkedList::IT_MODE_FIFO);
} catch (Exception $e) {
    echo 'unexpected exception: ' . $e->getMessage() . "\n";
}
try {
    $q->setIteratorMode(SplDoublyLinkedList::IT_MODE_LIFO);
} catch (Exception $e) {
    echo 'expected exception: ' . $e->getMessage() . "\n";
}
?>
===DONE===
--EXPECT--
expected exception: Iterators' LIFO/FIFO modes for SplStack/SplQueue objects are frozen
===DONE===
