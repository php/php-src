--TEST--
SPL: DoublyLinkedList: std operations
--FILE--
<?php
$dll = new SplDoublyLinkedList();
// errors
try {
    $dll->pop();
} catch (RuntimeException $e) {
    echo "Exception: ".$e->getMessage()."\n";
}
try {
    $dll->shift();
} catch (RuntimeException $e) {
    echo "Exception: ".$e->getMessage()."\n";
}

// data consistency
$a = 2;
$dll->push($a);
echo $dll->pop()."\n";

$a = 2;
$dll->unshift($a);
echo $dll->shift()."\n";

// peakable
$dll->push(1);
$dll->push(2);
echo $dll->top()."\n";
echo $dll->bottom()."\n";
$dll->pop();
$dll->pop();

// countable
$dll->push(NULL);
$dll->push(NULL);
echo count($dll)."\n";
echo $dll->count()."\n";
var_dump($dll->pop());
var_dump($dll->pop());

// clonable
$dll->push(2);
$dll_clone = clone $dll;
$dll_clone->pop();
echo count($dll)."\n";
?>
--EXPECT--
Exception: Can't pop from an empty datastructure
Exception: Can't shift from an empty datastructure
2
2
2
1
2
2
NULL
NULL
1
