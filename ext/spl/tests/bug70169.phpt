--TEST--
SPL: Bug #70169	Use After Free Vulnerability in unserialize() with SplDoublyLinkedList
--FILE--
<?php
$inner = 'i:1;';
$exploit = 'a:2:{i:0;C:19:"SplDoublyLinkedList":'.strlen($inner).':{'.$inner.'}i:1;R:3;}';

$data = unserialize($exploit);

for($i = 0; $i < 5; $i++) {
    $v[$i] = 'hi'.$i;
}

var_dump($data);
?>
===DONE===
--EXPECTF--
array(2) {
  [0]=>
  object(SplDoublyLinkedList)#%d (2) {
    ["flags":"SplDoublyLinkedList":private]=>
    int(1)
    ["dllist":"SplDoublyLinkedList":private]=>
    array(0) {
    }
  }
  [1]=>
  int(1)
}
===DONE===
