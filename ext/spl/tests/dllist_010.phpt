--TEST--
SPL: DoublyLinkedList: prev
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
$dll->prev();
echo $dll->current()."\n";

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
1
3
2
===DONE===
