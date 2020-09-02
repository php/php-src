--TEST--
Check that SplDoublyLinkedList::add throws an exception with an invalid offset argument
--FILE--
<?php
try {
    $dll = new SplDoublyLinkedList();
    var_dump($dll->add([],2));
} catch (TypeError $e) {
    echo "Exception: ".$e->getMessage()."\n";
}
?>
--EXPECT--
Exception: SplDoublyLinkedList::add(): Argument #1 ($index) must be of type int, array given
