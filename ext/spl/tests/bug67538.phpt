--TEST--
Bug #67538 (SPL Iterators use-after-free)
--FILE--
<?php
$list = new SplDoublyLinkedList();
$list->push('a');
$list->push('b');

$list->rewind();
$list->offsetUnset(0);
$list->push('b');
$list->offsetUnset(0);
$list->next();
echo "okey";
?>
--EXPECTF--
okey
