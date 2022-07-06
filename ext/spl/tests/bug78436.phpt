--TEST--
Bug #78436: Missing addref in SplPriorityQueue EXTR_BOTH mode
--FILE--
<?php

$pq = new SplPriorityQueue();
$pq->insert(new stdClass, 1);
var_dump($pq);
var_dump($pq);

?>
--EXPECT--
object(SplPriorityQueue)#1 (3) {
  ["flags":"SplPriorityQueue":private]=>
  int(1)
  ["isCorrupted":"SplPriorityQueue":private]=>
  bool(false)
  ["heap":"SplPriorityQueue":private]=>
  array(1) {
    [0]=>
    array(2) {
      ["data"]=>
      object(stdClass)#2 (0) {
      }
      ["priority"]=>
      int(1)
    }
  }
}
object(SplPriorityQueue)#1 (3) {
  ["flags":"SplPriorityQueue":private]=>
  int(1)
  ["isCorrupted":"SplPriorityQueue":private]=>
  bool(false)
  ["heap":"SplPriorityQueue":private]=>
  array(1) {
    [0]=>
    array(2) {
      ["data"]=>
      object(stdClass)#2 (0) {
      }
      ["priority"]=>
      int(1)
    }
  }
}
