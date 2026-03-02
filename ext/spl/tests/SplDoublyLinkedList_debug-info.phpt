--TEST--
Check that SplDoublyLinkedList returns debug info when print_r is used.
--CREDITS--
PHPNW Testfest 2009 - Paul Court ( g@rgoyle.com )
--FILE--
<?php
    // Create a new Doubly Linked List
    $dll = new SplDoublyLinkedList();

    // Add some items to the list
    $dll->push(1);
    $dll->push(2);
    $dll->push(3);

    // Check the debug info
    print_r($dll);
?>
--EXPECT--
SplDoublyLinkedList Object
(
    [flags:SplDoublyLinkedList:private] => 0
    [dllist:SplDoublyLinkedList:private] => Array
        (
            [0] => 1
            [1] => 2
            [2] => 3
        )

)
