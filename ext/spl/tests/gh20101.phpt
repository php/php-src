--TEST--
GH-20101 (SplHeap/SplPriorityQueue serialization exposes INDIRECTs)
--FILE--
<?php
class CustomHeap extends SplMaxHeap {
    public $field = 0;
}
$heap = new CustomHeap();
$data = $heap->__serialize();
var_dump($data);

class CustomPriorityQueue extends SplPriorityQueue {
    public $field = 0;
}
$pqueue = new CustomPriorityQueue();
$data = $pqueue->__serialize();
var_dump($data);
?>
--EXPECT--
array(2) {
  [0]=>
  array(1) {
    ["field"]=>
    int(0)
  }
  [1]=>
  array(2) {
    ["flags"]=>
    int(0)
    ["heap_elements"]=>
    array(0) {
    }
  }
}
array(2) {
  [0]=>
  array(1) {
    ["field"]=>
    int(0)
  }
  [1]=>
  array(2) {
    ["flags"]=>
    int(1)
    ["heap_elements"]=>
    array(0) {
    }
  }
}
