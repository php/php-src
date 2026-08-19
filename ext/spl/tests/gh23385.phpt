--TEST--
GH-23385 (Use-after-free in SplDoublyLinkedList::serialize())
--FILE--
<?php
class RemoveSelf {
    public function __serialize(): array {
        global $list;
        unset($list[0]);
        return [];
    }
}

$list = new SplDoublyLinkedList();
$list->push([new RemoveSelf(), [1, 2, 3]]);
$list->push("tail");
var_dump($list->serialize());
var_dump($list->count());

class RemoveNext {
    public function __serialize(): array {
        global $list2;
        unset($list2[1]);
        return [];
    }
}

$list2 = new SplDoublyLinkedList();
$list2->push(new RemoveNext());
$list2->push("removed");
$list2->push("after");
var_dump($list2->serialize());
var_dump($list2->count());

class RemoveAll {
    public function __serialize(): array {
        global $list3;
        while (!$list3->isEmpty()) {
            $list3->pop();
        }
        return [];
    }
}

$list3 = new SplDoublyLinkedList();
$list3->push([new RemoveAll(), [1, 2]]);
$list3->push("x");
$list3->push("y");
var_dump($list3->serialize());
var_dump($list3->count());
?>
--EXPECTF--
string(%d) "i:0;:a:2:{i:0;O:10:"RemoveSelf":0:{}i:1;a:3:{i:0;i:1;i:1;i:2;i:2;i:3;}}:s:4:"tail";"
int(1)
string(%d) "i:0;:O:10:"RemoveNext":0:{}"
int(2)
string(%d) "i:0;:a:2:{i:0;O:9:"RemoveAll":0:{}i:1;a:2:{i:0;i:1;i:1;i:2;}}"
int(0)
