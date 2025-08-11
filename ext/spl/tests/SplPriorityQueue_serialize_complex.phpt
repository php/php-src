--TEST--
SplPriorityQueue serialization with complex data types
--FILE--
<?php
$queue = new SplPriorityQueue();

$array1 = ['name' => 'John', 'age' => 30, 'hobbies' => ['reading', 'gaming']];
$array2 = [1, 2, [3, 4, ['nested' => true]]];
$queue->insert($array1, 10);
$queue->insert($array2, 5);

class TestClass {
    public $prop = 'test';
    private $private = 'hidden';
    
    public function __construct($value = null) {
        if ($value) $this->prop = $value;
    }
}

$obj1 = new TestClass('object1');
$obj2 = new stdClass();
$obj2->data = 'standard object';
$obj2->number = 42;

$queue->insert($obj1, 15);
$queue->insert($obj2, 8);

$queue->insert(3.14159, 12);
$queue->insert(true, 20);
$queue->insert(false, 1);
$queue->insert(null, 3);

echo "Original queue count: " . count($queue) . "\n";

$serialized = serialize($queue);
$unserialized = unserialize($serialized);

echo "Unserialized queue count: " . count($unserialized) . "\n";

while (!$unserialized->isEmpty()) {
    $item = $unserialized->extract();
    
    if (is_array($item)) {
        echo "Array: " . json_encode($item) . "\n";
    } elseif (is_object($item)) {
        if ($item instanceof TestClass) {
            echo "TestClass object: prop=" . $item->prop . "\n";
        } elseif ($item instanceof stdClass) {
            echo "stdClass object: data=" . $item->data . ", number=" . $item->number . "\n";
        }
    } elseif (is_bool($item)) {
        echo "Boolean: " . ($item ? "true" : "false") . "\n";
    } elseif (is_null($item)) {
        echo "NULL value\n";
    } elseif (is_float($item)) {
        echo "Float: " . $item . "\n";
    } else {
        echo "Other: " . var_export($item, true) . "\n";
    }
}

?>
--EXPECT--
Original queue count: 8
Unserialized queue count: 8
Boolean: true
TestClass object: prop=object1
Float: 3.14159
Array: {"name":"John","age":30,"hobbies":["reading","gaming"]}
stdClass object: data=standard object, number=42
Array: [1,2,[3,4,{"nested":true}]]
NULL value
Boolean: false
