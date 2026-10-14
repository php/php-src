--TEST--
GC buffer shouldn't get reused when removing nested data — under test_scheduler
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
$s = <<<'STR'
O:8:"stdClass":2:{i:5;C:8:"SplStack":29:{i:4;:r:1;:O:8:"stdClass":0:{}}i:0;O:13:"RegexIterator":1:{i:5;C:8:"SplStack":29:{i:4;:r:1;:O:8:"stdClass":0:{}}}}
STR;
var_dump(unserialize($s));
gc_collect_cycles();
?>
--EXPECTF--
Deprecated: Creation of dynamic property RegexIterator::$5 is deprecated in %s on line %d
object(stdClass)#%d (2) {
  ["5"]=>
  object(SplStack)#%d (2) {
    ["flags":"SplDoublyLinkedList":private]=>
    int(4)
    ["dllist":"SplDoublyLinkedList":private]=>
    array(2) {
      [0]=>
      *RECURSION*
      [1]=>
      object(stdClass)#%d (0) {
      }
    }
  }
  ["0"]=>
  object(RegexIterator)#%d (2) {
    ["replacement"]=>
    NULL
    ["5"]=>
    object(SplStack)#%d (2) {
      ["flags":"SplDoublyLinkedList":private]=>
      int(4)
      ["dllist":"SplDoublyLinkedList":private]=>
      array(2) {
        [0]=>
        *RECURSION*
        [1]=>
        object(stdClass)#%d (0) {
        }
      }
    }
  }
}
