--TEST--
SplHeap serialization format - indexed array format prevents conflicts
--FILE--
<?php

$heap = new SplMaxHeap();
$heap->insert(100);
$heap->insert(50);
$heap->insert(200);

$serialized_data = $heap->__serialize();
var_dump($serialized_data);

$pq = new SplPriorityQueue();
$pq->insert('data', 10);
$pq->setExtractFlags(SplPriorityQueue::EXTR_DATA);

$pq_data = $pq->__serialize();
var_dump($pq_data);

class CustomHeap extends SplMaxHeap {
    public $flags = 'user_property';
    public $heap_elements = 'user_property';
}

$custom = new CustomHeap();
$custom->insert(42);

$custom_data = $custom->__serialize();
var_dump($custom_data);

?>
--EXPECT--
array(2) {
  [0]=>
  array(0) {
  }
  [1]=>
  array(2) {
    ["flags"]=>
    int(0)
    ["heap_elements"]=>
    array(3) {
      [0]=>
      int(200)
      [1]=>
      int(50)
      [2]=>
      int(100)
    }
  }
}
array(2) {
  [0]=>
  array(0) {
  }
  [1]=>
  array(2) {
    ["flags"]=>
    int(1)
    ["heap_elements"]=>
    array(1) {
      [0]=>
      array(2) {
        ["data"]=>
        string(4) "data"
        ["priority"]=>
        int(10)
      }
    }
  }
}
array(2) {
  [0]=>
  array(2) {
    ["flags"]=>
    string(13) "user_property"
    ["heap_elements"]=>
    string(13) "user_property"
  }
  [1]=>
  array(2) {
    ["flags"]=>
    int(0)
    ["heap_elements"]=>
    array(1) {
      [0]=>
      int(42)
    }
  }
}
