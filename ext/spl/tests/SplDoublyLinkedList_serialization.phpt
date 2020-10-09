--TEST--
Check Serialization/unserialization
--FILE--
<?php
$q = new SplQueue();

$q->enqueue("a");
$q->enqueue("b");

var_dump($q, $ss = serialize($q), unserialize($ss));

$s = new SplStack();

$s->push("a");
$s->push("b");

var_dump($s, $ss = serialize($s), unserialize($ss));
?>
==END==
--EXPECTF--
object(SplQueue)#%d (2) {
  ["flags":"SplDoublyLinkedList":private]=>
  int(4)
  ["dllist":"SplDoublyLinkedList":private]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "b"
  }
}
string(71) "O:8:"SplQueue":3:{i:0;i:4;i:1;a:2:{i:0;s:1:"a";i:1;s:1:"b";}i:2;a:0:{}}"
object(SplQueue)#%d (2) {
  ["flags":"SplDoublyLinkedList":private]=>
  int(4)
  ["dllist":"SplDoublyLinkedList":private]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "b"
  }
}
object(SplStack)#%d (2) {
  ["flags":"SplDoublyLinkedList":private]=>
  int(6)
  ["dllist":"SplDoublyLinkedList":private]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "b"
  }
}
string(71) "O:8:"SplStack":3:{i:0;i:6;i:1;a:2:{i:0;s:1:"a";i:1;s:1:"b";}i:2;a:0:{}}"
object(SplStack)#%d (2) {
  ["flags":"SplDoublyLinkedList":private]=>
  int(6)
  ["dllist":"SplDoublyLinkedList":private]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "b"
  }
}
==END==
