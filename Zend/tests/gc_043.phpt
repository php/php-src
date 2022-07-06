--TEST--
GC buffer shouldn't get reused when removing nested data
--FILE--
<?php
$s = <<<'STR'
O:8:"stdClass":2:{i:5;C:8:"SplStack":29:{i:4;:r:1;:O:8:"stdClass":0:{}}i:0;O:13:"RegexIterator":1:{i:5;C:8:"SplStack":29:{i:4;:r:1;:O:8:"stdClass":0:{}}}}
STR;
var_dump(unserialize($s));
gc_collect_cycles();
?>
--EXPECT--
object(stdClass)#1 (2) {
  ["5"]=>
  object(SplStack)#2 (2) {
    ["flags":"SplDoublyLinkedList":private]=>
    int(4)
    ["dllist":"SplDoublyLinkedList":private]=>
    array(2) {
      [0]=>
      *RECURSION*
      [1]=>
      object(stdClass)#3 (0) {
      }
    }
  }
  ["0"]=>
  object(RegexIterator)#4 (2) {
    ["replacement"]=>
    NULL
    ["5"]=>
    object(SplStack)#5 (2) {
      ["flags":"SplDoublyLinkedList":private]=>
      int(4)
      ["dllist":"SplDoublyLinkedList":private]=>
      array(2) {
        [0]=>
        *RECURSION*
        [1]=>
        object(stdClass)#6 (0) {
        }
      }
    }
  }
}
