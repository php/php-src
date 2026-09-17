--TEST--
GH-23385 (Use-after-free in SplDoublyLinkedList::serialize())
--CREDITS--
f9j2n6nd8k-eng
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

class RemoveHolder {
    public function __serialize(): array {
        global $list4;
        unset($list4[0]);
        return [];
    }
}

class Holder {
    public $first;
    public $second = "second";
    public $third = "third";
}

$holder = new Holder();
$holder->first = new RemoveHolder();

$list4 = new SplDoublyLinkedList();
$list4->push($holder);
unset($holder);
$list4->push("tail");
var_dump($list4->serialize());
var_dump($list4->count());

?>
--EXPECT--
string(83) "i:0;:a:2:{i:0;O:10:"RemoveSelf":0:{}i:1;a:3:{i:0;i:1;i:1;i:2;i:2;i:3;}}:s:4:"tail";"
int(1)
string(27) "i:0;:O:10:"RemoveNext":0:{}"
int(2)
string(61) "i:0;:a:2:{i:0;O:9:"RemoveAll":0:{}i:1;a:2:{i:0;i:1;i:1;i:2;}}"
int(0)
string(120) "i:0;:O:6:"Holder":3:{s:5:"first";O:12:"RemoveHolder":0:{}s:6:"second";s:6:"second";s:5:"third";s:5:"third";}:s:4:"tail";"
int(1)
