--TEST--
SplHeap serialization binary format
--FILE--
<?php

// Test SplMaxHeap empty
$heap = new SplMaxHeap();
$s = serialize($heap);
echo $s . "\n";
var_dump(unserialize($s));

// Test SplMaxHeap with data  
$heap = new SplMaxHeap();
$heap->insert(100);
$heap->insert(50);
$heap->insert(200);
$s = serialize($heap);
echo $s . "\n";
$unserialized = unserialize($s);
var_dump($unserialized);

// Test SplMinHeap
$min = new SplMinHeap();
$min->insert(30);
$min->insert(10);
$min->insert(20);
$s = serialize($min);
echo $s . "\n";
var_dump(unserialize($s));

// Test SplPriorityQueue empty
$pq = new SplPriorityQueue();
$s = serialize($pq);
echo $s . "\n";
var_dump(unserialize($s));

// Test SplPriorityQueue with data
$pq = new SplPriorityQueue();
$pq->insert('low', 1);
$pq->insert('high', 10);
$pq->insert('medium', 5);
$pq->setExtractFlags(SplPriorityQueue::EXTR_BOTH);
$s = serialize($pq);
echo $s . "\n";
var_dump(unserialize($s));

?>
--EXPECT--
O:10:"SplMaxHeap":2:{i:0;a:0:{}i:1;a:2:{s:5:"flags";i:0;s:13:"heap_elements";a:0:{}}}
object(SplMaxHeap)#2 (3) {
  ["flags":"SplHeap":private]=>
  int(0)
  ["isCorrupted":"SplHeap":private]=>
  bool(false)
  ["heap":"SplHeap":private]=>
  array(0) {
  }
}
O:10:"SplMaxHeap":2:{i:0;a:0:{}i:1;a:2:{s:5:"flags";i:0;s:13:"heap_elements";a:3:{i:0;i:200;i:1;i:50;i:2;i:100;}}}
object(SplMaxHeap)#1 (3) {
  ["flags":"SplHeap":private]=>
  int(0)
  ["isCorrupted":"SplHeap":private]=>
  bool(false)
  ["heap":"SplHeap":private]=>
  array(3) {
    [0]=>
    int(200)
    [1]=>
    int(50)
    [2]=>
    int(100)
  }
}
O:10:"SplMinHeap":2:{i:0;a:0:{}i:1;a:2:{s:5:"flags";i:0;s:13:"heap_elements";a:3:{i:0;i:10;i:1;i:30;i:2;i:20;}}}
object(SplMinHeap)#4 (3) {
  ["flags":"SplHeap":private]=>
  int(0)
  ["isCorrupted":"SplHeap":private]=>
  bool(false)
  ["heap":"SplHeap":private]=>
  array(3) {
    [0]=>
    int(10)
    [1]=>
    int(30)
    [2]=>
    int(20)
  }
}
O:16:"SplPriorityQueue":2:{i:0;a:0:{}i:1;a:2:{s:5:"flags";i:1;s:13:"heap_elements";a:0:{}}}
object(SplPriorityQueue)#5 (3) {
  ["flags":"SplPriorityQueue":private]=>
  int(1)
  ["isCorrupted":"SplPriorityQueue":private]=>
  bool(false)
  ["heap":"SplPriorityQueue":private]=>
  array(0) {
  }
}
O:16:"SplPriorityQueue":2:{i:0;a:0:{}i:1;a:2:{s:5:"flags";i:3;s:13:"heap_elements";a:3:{i:0;a:2:{s:4:"data";s:4:"high";s:8:"priority";i:10;}i:1;a:2:{s:4:"data";s:3:"low";s:8:"priority";i:1;}i:2;a:2:{s:4:"data";s:6:"medium";s:8:"priority";i:5;}}}}
object(SplPriorityQueue)#4 (3) {
  ["flags":"SplPriorityQueue":private]=>
  int(3)
  ["isCorrupted":"SplPriorityQueue":private]=>
  bool(false)
  ["heap":"SplPriorityQueue":private]=>
  array(3) {
    [0]=>
    array(2) {
      ["data"]=>
      string(4) "high"
      ["priority"]=>
      int(10)
    }
    [1]=>
    array(2) {
      ["data"]=>
      string(3) "low"
      ["priority"]=>
      int(1)
    }
    [2]=>
    array(2) {
      ["data"]=>
      string(6) "medium"
      ["priority"]=>
      int(5)
    }
  }
}
