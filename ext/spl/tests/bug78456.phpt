--TEST--
Bug #78456: Segfault when serializing SplDoublyLinkedList
--FILE--
<?php

$dllist = new SplDoublyLinkedList();
$dllist->push(new stdClass);
var_dump($s = serialize($dllist));
var_dump(unserialize($s));

?>
--EXPECT--
string(88) "O:19:"SplDoublyLinkedList":3:{i:0;i:0;i:1;a:1:{i:0;O:13:"DynamicObject":0:{}}i:2;a:0:{}}"
object(SplDoublyLinkedList)#3 (2) {
  ["flags":"SplDoublyLinkedList":private]=>
  int(0)
  ["dllist":"SplDoublyLinkedList":private]=>
  array(1) {
    [0]=>
    object(DynamicObject)#4 (0) {
    }
  }
}
