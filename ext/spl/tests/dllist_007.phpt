--TEST--
SPL: DoublyLinkedList: Iterator
--FILE--
<?php
$a = new SplDoublyLinkedList();
$a->push(1);
$a->push(2);
$a->push(3);

$a->rewind();
while ($a->valid()) {
    var_dump($a->current(), $a->next());
}
?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
int(1)
NULL
int(2)
NULL
int(3)
NULL
===DONE===
