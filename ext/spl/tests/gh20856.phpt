--TEST--
GH-20856 (heap-use-after-free in SplDoublyLinkedList iterator when modifying during iteration)
--CREDITS--
vi3tL0u1s
iluuu1994
--FILE--
<?php
$m = new SplStack;
$m[] = new stdClass;
$m[] = new stdClass;

foreach ($m as $l) {
    unset($m[0]);
    unset($m[0]);
}

var_dump($m);
?>
--EXPECTF--
object(SplStack)#%d (%d) {
  ["flags":"SplDoublyLinkedList":private]=>
  int(6)
  ["dllist":"SplDoublyLinkedList":private]=>
  array(0) {
  }
}
