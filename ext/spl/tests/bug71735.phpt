--TEST--
Bug #71735 (Double-free in SplDoublyLinkedList::offsetSet)
--FILE--
<?php
try {
$var_1=new SplStack();
$var_1->offsetSet(100,new DateTime('2000-01-01'));
} catch(OutOfRangeException $e) {
    print $e->getMessage()."\n";
}
?>
--EXPECT--
SplDoublyLinkedList::offsetSet(): Argument #1 ($index) is out of range
