--TEST--
SPL: DoublyLinkedList: iterator modes
--FILE--
<?php
$dll = new SplDoublyLinkedList();
$dll->push(2);
$dll->push(3);
$dll->push(4);

$dll->setIteratorMode(SplDoublyLinkedList::IT_MODE_LIFO);

foreach ($dll as $k => $v) {
    echo "$k=>$v\n";
}

$dll->setIteratorMode(SplDoublyLinkedList::IT_MODE_FIFO);
foreach ($dll as $k => $v) {
    echo "$k=>$v\n";
}

$dll->setIteratorMode(SplDoublyLinkedList::IT_MODE_FIFO | SplDoublyLinkedList::IT_MODE_DELETE);
var_dump($dll->count());
foreach ($dll as $k => $v) {
    echo "$k=>$v\n";
}
var_dump($dll->count());

?>
--EXPECT--
2=>4
1=>3
0=>2
0=>2
1=>3
2=>4
int(3)
0=>2
0=>3
0=>4
int(0)
