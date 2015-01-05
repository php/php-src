--TEST--
SPL: SplPriorityQueue: top exception on empty heap
--CREDITS--
Nathaniel McHugh nat@fishtrap.co.uk
#testfest 2009-05-09
--FILE--
<?php

$priorityQueue = new SplPriorityQueue();

try {
    $priorityQueue->top();
} catch (RuntimeException $e) {
    echo "Exception: ".$e->getMessage().PHP_EOL;
}

?>
--EXPECT--
Exception: Can't peek at an empty heap
