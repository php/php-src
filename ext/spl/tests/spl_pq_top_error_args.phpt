--TEST--
SPL: SplPriorityQueue: top too many arguments exception
--CREDITS--
Nathaniel McHugh nat@fishtrap.co.uk
#testfest London 2009-05-09
--FILE--
<?php
$priorityQueue = new SplPriorityQueue();
$priorityQueue->top('var');
?>
--EXPECTF--
Warning: SplPriorityQueue::top() expects exactly 0 parameters, 1 given in %s