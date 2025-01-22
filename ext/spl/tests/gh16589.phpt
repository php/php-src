--TEST--
GH-16589 (UAF in SplDoublyLinked->serialize())
--CREDITS--
chibinz
--FILE--
<?php

class C {
    function __serialize(): array {
        global $list;
        $list->pop();
        return [];
    }
}

$list = new SplDoublyLinkedList;
$list->add(0, new C);
$list->add(1, 1);
var_dump($list->serialize());

?>
--EXPECT--
string(17) "i:0;:O:1:"C":0:{}"
