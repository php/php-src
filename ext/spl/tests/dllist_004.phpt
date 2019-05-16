--TEST--
SPL: DoublyLinkedList: Stacks
--FILE--
<?php
$stack = new SplStack();
// errors
try {
    $stack->pop();
} catch (RuntimeException $e) {
    echo "Exception: ".$e->getMessage()."\n";
}
try {
    $stack->shift();
} catch (RuntimeException $e) {
    echo "Exception: ".$e->getMessage()."\n";
}

// data consistency
$a = 2;
$stack->push($a);
echo $stack->pop()."\n";

// peakable
$stack->push(1);
$stack->push(2);
echo $stack->top()."\n";

// iterable
foreach ($stack as $elem) {
    echo "[$elem]\n";
}

// countable
$stack->push(NULL);
$stack->push(NULL);
echo count($stack)."\n";
echo $stack->count()."\n";
var_dump($stack->pop());
var_dump($stack->pop());

// clonable
$stack->push(2);
$stack_clone = clone $stack;
$stack_clone->pop();
echo count($stack)."\n";
?>
===DONE===
<?php exit(0); ?>
--EXPECT--
Exception: Can't pop from an empty datastructure
Exception: Can't shift from an empty datastructure
2
2
[2]
[1]
4
4
NULL
NULL
3
===DONE===
