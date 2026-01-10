--TEST--
SplPriorityQueue serialization with mixed data types and extract flags
--FILE--
<?php
$queue = new SplPriorityQueue();
$queue->setExtractFlags(SplPriorityQueue::EXTR_BOTH);

$array1 = ['name' => 'John', 'hobbies' => ['reading', 'gaming']];
$queue->insert($array1, 10);

class TestClass {
    public $prop = 'test';
}
$obj1 = new TestClass();
$queue->insert($obj1, 15);

$queue->insert(3.14159, 12);
$queue->insert(true, 20);
$queue->insert(null, 3);

$serialized = serialize($queue);
echo $serialized . "\n";

$unserialized = unserialize($serialized);
var_dump($unserialized);

$queue2 = new SplPriorityQueue();
$queue2->setExtractFlags(SplPriorityQueue::EXTR_PRIORITY);
$queue2->insert("data", 42);

$serialized2 = serialize($queue2);
echo $serialized2 . "\n";

$unserialized2 = unserialize($serialized2);
var_dump($unserialized2);

?>
--EXPECT--
O:16:"SplPriorityQueue":2:{i:0;a:0:{}i:1;a:2:{s:5:"flags";i:3;s:13:"heap_elements";a:5:{i:0;a:2:{s:4:"data";b:1;s:8:"priority";i:20;}i:1;a:2:{s:4:"data";O:9:"TestClass":1:{s:4:"prop";s:4:"test";}s:8:"priority";i:15;}i:2;a:2:{s:4:"data";d:3.14159;s:8:"priority";i:12;}i:3;a:2:{s:4:"data";a:2:{s:4:"name";s:4:"John";s:7:"hobbies";a:2:{i:0;s:7:"reading";i:1;s:6:"gaming";}}s:8:"priority";i:10;}i:4;a:2:{s:4:"data";N;s:8:"priority";i:3;}}}}
object(SplPriorityQueue)#3 (3) {
  ["flags":"SplPriorityQueue":private]=>
  int(3)
  ["isCorrupted":"SplPriorityQueue":private]=>
  bool(false)
  ["heap":"SplPriorityQueue":private]=>
  array(5) {
    [0]=>
    array(2) {
      ["data"]=>
      bool(true)
      ["priority"]=>
      int(20)
    }
    [1]=>
    array(2) {
      ["data"]=>
      object(TestClass)#4 (1) {
        ["prop"]=>
        string(4) "test"
      }
      ["priority"]=>
      int(15)
    }
    [2]=>
    array(2) {
      ["data"]=>
      float(3.14159)
      ["priority"]=>
      int(12)
    }
    [3]=>
    array(2) {
      ["data"]=>
      array(2) {
        ["name"]=>
        string(4) "John"
        ["hobbies"]=>
        array(2) {
          [0]=>
          string(7) "reading"
          [1]=>
          string(6) "gaming"
        }
      }
      ["priority"]=>
      int(10)
    }
    [4]=>
    array(2) {
      ["data"]=>
      NULL
      ["priority"]=>
      int(3)
    }
  }
}
O:16:"SplPriorityQueue":2:{i:0;a:0:{}i:1;a:2:{s:5:"flags";i:2;s:13:"heap_elements";a:1:{i:0;a:2:{s:4:"data";s:4:"data";s:8:"priority";i:42;}}}}
object(SplPriorityQueue)#6 (3) {
  ["flags":"SplPriorityQueue":private]=>
  int(2)
  ["isCorrupted":"SplPriorityQueue":private]=>
  bool(false)
  ["heap":"SplPriorityQueue":private]=>
  array(1) {
    [0]=>
    array(2) {
      ["data"]=>
      string(4) "data"
      ["priority"]=>
      int(42)
    }
  }
}
