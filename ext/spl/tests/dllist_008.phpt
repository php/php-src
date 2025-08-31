--TEST--
SPL: SplDoublyLinkedList with overridden count()
--FILE--
<?php
$obj = new SplDoublyLinkedList();
$obj[] = 1;
$obj[] = 2;
var_dump(count($obj));
class SplDoublyLinkedList2 extends SplDoublyLinkedList{
    public function count(): int {
        return -parent::count();
    }
}
$obj = new SplDoublyLinkedList2();
$obj[] = 1;
$obj[] = 2;
var_dump(count($obj));
?>
--EXPECT--
int(2)
int(-2)
