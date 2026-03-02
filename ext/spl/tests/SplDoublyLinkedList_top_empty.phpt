--TEST--
SplDoublyLinkedList::top empty
--CREDITS--
Gabriel Caruso (carusogabriel34@gmail.com)
--FILE--
<?php
try {
    (new SplDoublyLinkedList)->top();
} catch (RuntimeException $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
Can't peek at an empty datastructure
