--TEST--
SPL: DoublyLinkedList: memory leak when iterator pointer isn't at the last element
--FILE--
<?php
$dll = new SplDoublyLinkedList();
$dll->push(1);
$dll->push(2);
$dll->push(3);
$dll->push(4);


$dll->rewind();
echo $dll->current()."\n";
$dll->next();
$dll->next();
echo $dll->current()."\n";

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
1
3
===DONE===
