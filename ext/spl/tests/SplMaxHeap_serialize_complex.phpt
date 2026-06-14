--TEST--
SplMaxHeap serialization with complex data types
--FILE--
<?php
$heap = new SplMaxHeap();
$heap->insert(['type' => 'array1', 'value' => 10]);
$heap->insert(['type' => 'array2', 'value' => 20]);
$heap->insert(['type' => 'array3', 'value' => 5]);

$serialized = serialize($heap);
echo $serialized . "\n";

$unserialized = unserialize($serialized);
var_dump($unserialized);

class TestObj {
    public $val;
    public function __construct($v) { $this->val = $v; }
}

$heap2 = new SplMaxHeap();
$heap2->insert(new TestObj(30));
$heap2->insert(new TestObj(10));
$heap2->insert(new TestObj(20));

$serialized2 = serialize($heap2);
echo $serialized2 . "\n";

$unserialized2 = unserialize($serialized2);
var_dump($unserialized2);

?>
--EXPECT--
O:10:"SplMaxHeap":2:{i:0;a:0:{}i:1;a:2:{s:5:"flags";i:0;s:13:"heap_elements";a:3:{i:0;a:2:{s:4:"type";s:6:"array3";s:5:"value";i:5;}i:1;a:2:{s:4:"type";s:6:"array1";s:5:"value";i:10;}i:2;a:2:{s:4:"type";s:6:"array2";s:5:"value";i:20;}}}}
object(SplMaxHeap)#2 (3) {
  ["flags":"SplHeap":private]=>
  int(0)
  ["isCorrupted":"SplHeap":private]=>
  bool(false)
  ["heap":"SplHeap":private]=>
  array(3) {
    [0]=>
    array(2) {
      ["type"]=>
      string(6) "array3"
      ["value"]=>
      int(5)
    }
    [1]=>
    array(2) {
      ["type"]=>
      string(6) "array1"
      ["value"]=>
      int(10)
    }
    [2]=>
    array(2) {
      ["type"]=>
      string(6) "array2"
      ["value"]=>
      int(20)
    }
  }
}
O:10:"SplMaxHeap":2:{i:0;a:0:{}i:1;a:2:{s:5:"flags";i:0;s:13:"heap_elements";a:3:{i:0;O:7:"TestObj":1:{s:3:"val";i:30;}i:1;O:7:"TestObj":1:{s:3:"val";i:10;}i:2;O:7:"TestObj":1:{s:3:"val";i:20;}}}}
object(SplMaxHeap)#7 (3) {
  ["flags":"SplHeap":private]=>
  int(0)
  ["isCorrupted":"SplHeap":private]=>
  bool(false)
  ["heap":"SplHeap":private]=>
  array(3) {
    [0]=>
    object(TestObj)#8 (1) {
      ["val"]=>
      int(30)
    }
    [1]=>
    object(TestObj)#9 (1) {
      ["val"]=>
      int(10)
    }
    [2]=>
    object(TestObj)#10 (1) {
      ["val"]=>
      int(20)
    }
  }
}
