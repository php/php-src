--TEST--
SplQueue setIteratorMode to LIFO produces fail condition in try/catch
--CREDITS--
PHPNW Test Fest 2009 - Jeremy Coates jeremy@phpnw.org.uk
--FILE--
<?php

try {

    $dll = new SplQueue();
    $dll->setIteratorMode(SplDoublyLinkedList::IT_MODE_LIFO);

} catch (Exception $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
Iterators' LIFO/FIFO modes for SplStack/SplQueue objects are frozen
