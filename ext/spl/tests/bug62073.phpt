--TEST--
Bug #62073 (different ways of iterating over an SplMaxHeap result in different keys)
--FILE--
<?php
$heap = new SplMaxHeap();
$heap->insert(42);
foreach ($heap as $key => $value) {
    var_dump($key);
    var_dump($value);
    break;
}

$heap = new SplMaxHeap();
$heap->insert(42);
var_dump($heap->key());
var_dump($heap->current());
?>
--EXPECT--
int(0)
int(42)
int(0)
int(42)
