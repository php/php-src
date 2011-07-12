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
string(42) "C:8:"SplQueue":22:{i:4;:s:1:"a";:s:1:"b";}"
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
string(42) "C:8:"SplStack":22:{i:6;:s:1:"a";:s:1:"b";}"
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
