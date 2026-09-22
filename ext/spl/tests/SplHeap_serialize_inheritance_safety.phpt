--TEST--
SplHeap and SplPriorityQueue inheritance safety with conflicting property names
--FILE--
<?php

class CustomHeap extends SplMaxHeap {
    public $flags = 'user_flags_property';
    public $heap_elements = 'user_heap_elements_property';
    public $custom_prop = 'custom_value';
    protected $protected_prop = 'protected_value';
    private $private_prop = 'private_value';
}

$heap = new CustomHeap();
$heap->flags = 'modified_user_flags';
$heap->heap_elements = 'modified_user_elements';
$heap->custom_prop = 'modified_custom';

$heap->insert(100);
$heap->insert(50);
$heap->insert(200);

$serialized = serialize($heap);
echo $serialized . "\n";
$unserialized = unserialize($serialized);
var_dump($unserialized);

class CustomPriorityQueue extends SplPriorityQueue {
    public $flags = 'user_flags_property';
    public $heap_elements = 'user_heap_elements_property';
    public $custom_data = 'custom_data_value';
    protected $protected_priority = 'protected_priority_value';
}

$pq = new CustomPriorityQueue();
$pq->flags = 'modified_user_flags';
$pq->heap_elements = 'modified_user_elements';
$pq->custom_data = 'modified_custom_data';

$pq->insert('low_priority', 1);
$pq->insert('high_priority', 10);
$pq->insert('medium_priority', 5);

$pq->setExtractFlags(SplPriorityQueue::EXTR_BOTH);

$serialized_pq = serialize($pq);
echo $serialized_pq . "\n";
$unserialized_pq = unserialize($serialized_pq);
var_dump($unserialized_pq);

?>
--EXPECTF--
O:10:"CustomHeap":2:{i:0;a:5:{s:5:"flags";s:19:"modified_user_flags";s:13:"heap_elements";s:22:"modified_user_elements";s:11:"custom_prop";s:15:"modified_custom";s:17:"%0*%0protected_prop";s:15:"protected_value";s:24:"%0CustomHeap%0private_prop";s:13:"private_value";}i:1;a:2:{s:5:"flags";i:0;s:13:"heap_elements";a:3:{i:0;i:200;i:1;i:50;i:2;i:100;}}}
object(CustomHeap)#2 (8) {
  ["flags"]=>
  string(19) "modified_user_flags"
  ["heap_elements"]=>
  string(22) "modified_user_elements"
  ["custom_prop"]=>
  string(15) "modified_custom"
  ["protected_prop":protected]=>
  string(15) "protected_value"
  ["private_prop":"CustomHeap":private]=>
  string(13) "private_value"
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
O:19:"CustomPriorityQueue":2:{i:0;a:4:{s:5:"flags";s:19:"modified_user_flags";s:13:"heap_elements";s:22:"modified_user_elements";s:11:"custom_data";s:20:"modified_custom_data";s:21:"%0*%0protected_priority";s:24:"protected_priority_value";}i:1;a:2:{s:5:"flags";i:3;s:13:"heap_elements";a:3:{i:0;a:2:{s:4:"data";s:13:"high_priority";s:8:"priority";i:10;}i:1;a:2:{s:4:"data";s:12:"low_priority";s:8:"priority";i:1;}i:2;a:2:{s:4:"data";s:15:"medium_priority";s:8:"priority";i:5;}}}}
object(CustomPriorityQueue)#4 (7) {
  ["flags"]=>
  string(19) "modified_user_flags"
  ["heap_elements"]=>
  string(22) "modified_user_elements"
  ["custom_data"]=>
  string(20) "modified_custom_data"
  ["protected_priority":protected]=>
  string(24) "protected_priority_value"
  ["flags":"SplPriorityQueue":private]=>
  int(3)
  ["isCorrupted":"SplPriorityQueue":private]=>
  bool(false)
  ["heap":"SplPriorityQueue":private]=>
  array(3) {
    [0]=>
    array(2) {
      ["data"]=>
      string(13) "high_priority"
      ["priority"]=>
      int(10)
    }
    [1]=>
    array(2) {
      ["data"]=>
      string(12) "low_priority"
      ["priority"]=>
      int(1)
    }
    [2]=>
    array(2) {
      ["data"]=>
      string(15) "medium_priority"
      ["priority"]=>
      int(5)
    }
  }
}
