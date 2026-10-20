--TEST--
GH-22062 (SplDoublyLinkedList iterator UAF via destructor releasing next node)
--FILE--
<?php
$list = new SplDoublyLinkedList();
$list->setIteratorMode(
    SplDoublyLinkedList::IT_MODE_FIFO |
    SplDoublyLinkedList::IT_MODE_DELETE
);

$list->push(new class($list) {
    public function __construct(private SplDoublyLinkedList $list) {}
    public function __destruct() {
        if ($this->list->count() > 0) {
            $this->list->offsetUnset(0);
        }
    }
});

$list->push(new stdClass());

foreach ($list as $item) {
    unset($item);
}

var_dump($list->count());
?>
--EXPECT--
int(0)
