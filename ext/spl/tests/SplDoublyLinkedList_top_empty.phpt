--TEST--
SplDoublyLinkedList::top empty
--CREDITS--
Gabriel Caruso (carusogabriel34@gmail.com)
--FILE--
<?php
try {
    (new SplDoublyLinkedList)->top();
} catch (RuntimeException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
RuntimeException: Can't peek at an empty datastructure
