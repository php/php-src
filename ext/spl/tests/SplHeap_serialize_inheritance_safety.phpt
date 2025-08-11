--TEST--
SplHeap and SplPriorityQueue inheritance safety
--FILE--
<?php

class CustomHeap extends SplMaxHeap {
    public $flags = 'user_flags_property';
    public $heap_elements = 'user_heap_elements_property';
    public $custom_prop = 'custom_value';
    protected $protected_prop = 'protected_value';
    private $private_prop = 'private_value';
    
    public function getProtected() {
        return $this->protected_prop;
    }
}

$heap = new CustomHeap();
$heap->flags = 'modified_user_flags';
$heap->heap_elements = 'modified_user_elements';
$heap->custom_prop = 'modified_custom';

$heap->insert(100);
$heap->insert(50);
$heap->insert(200);

echo "Before serialization:\n";
echo "  flags property: " . $heap->flags . "\n";
echo "  heap_elements property: " . $heap->heap_elements . "\n";
echo "  custom_prop: " . $heap->custom_prop . "\n";
echo "  protected_prop: " . $heap->getProtected() . "\n";
echo "  heap count: " . count($heap) . "\n";

$serialized = serialize($heap);
$unserialized = unserialize($serialized);

echo "\nAfter unserialization:\n";
echo "  flags property: " . $unserialized->flags . "\n";
echo "  heap_elements property: " . $unserialized->heap_elements . "\n";
echo "  custom_prop: " . $unserialized->custom_prop . "\n";
echo "  protected_prop: " . $unserialized->getProtected() . "\n";
echo "  heap count: " . count($unserialized) . "\n";

$extracted = [];
while (!$unserialized->isEmpty()) {
    $extracted[] = $unserialized->extract();
}
echo "  extraction order: " . implode(', ', $extracted) . "\n";

class CustomPriorityQueue extends SplPriorityQueue {
    public $flags = 'user_flags_property';
    public $heap_elements = 'user_heap_elements_property';
    public $custom_data = 'custom_data_value';
    protected $protected_priority = 'protected_priority_value';
    
    public function getProtected() {
        return $this->protected_priority;
    }
}

$pq = new CustomPriorityQueue();
$pq->flags = 'modified_user_flags';
$pq->heap_elements = 'modified_user_elements';
$pq->custom_data = 'modified_custom_data';

$pq->insert('low_priority', 1);
$pq->insert('high_priority', 10);
$pq->insert('medium_priority', 5);

$pq->setExtractFlags(SplPriorityQueue::EXTR_BOTH);

echo "\nBefore serialization:\n";
echo "  flags property: " . $pq->flags . "\n";
echo "  heap_elements property: " . $pq->heap_elements . "\n";
echo "  custom_data: " . $pq->custom_data . "\n";
echo "  protected_priority: " . $pq->getProtected() . "\n";
echo "  extract flags: " . $pq->getExtractFlags() . "\n";
echo "  queue count: " . count($pq) . "\n";

$serialized_pq = serialize($pq);
$unserialized_pq = unserialize($serialized_pq);

echo "\nAfter unserialization:\n";
echo "  flags property: " . $unserialized_pq->flags . "\n";
echo "  heap_elements property: " . $unserialized_pq->heap_elements . "\n";
echo "  custom_data: " . $unserialized_pq->custom_data . "\n";
echo "  protected_priority: " . $unserialized_pq->getProtected() . "\n";
echo "  extract flags: " . $unserialized_pq->getExtractFlags() . "\n";
echo "  queue count: " . count($unserialized_pq) . "\n";

$result = $unserialized_pq->extract();
echo "  extracted with EXTR_BOTH - data: " . $result['data'] . ", priority: " . $result['priority'] . "\n";

?>
--EXPECT--
Before serialization:
  flags property: modified_user_flags
  heap_elements property: modified_user_elements
  custom_prop: modified_custom
  protected_prop: protected_value
  heap count: 3

After unserialization:
  flags property: modified_user_flags
  heap_elements property: modified_user_elements
  custom_prop: modified_custom
  protected_prop: protected_value
  heap count: 3
  extraction order: 200, 100, 50

Before serialization:
  flags property: modified_user_flags
  heap_elements property: modified_user_elements
  custom_data: modified_custom_data
  protected_priority: protected_priority_value
  extract flags: 3
  queue count: 3

After unserialization:
  flags property: modified_user_flags
  heap_elements property: modified_user_elements
  custom_data: modified_custom_data
  protected_priority: protected_priority_value
  extract flags: 3
  queue count: 3
  extracted with EXTR_BOTH - data: high_priority, priority: 10
