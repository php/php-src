--TEST--
SPL: priorityQueue paramter test on insert method
--CREDITS--
Sean Burlington www.practicalweb.co.uk
TestFest London May 2009
--FILE--
<?php


$testHeap = new SplPriorityQueue();


var_dump($testHeap->insert());
var_dump($testHeap->insert('test'));
var_dump($testHeap->insert('test', 'test'));
var_dump($testHeap->insert('test', 'test', 'test'));


?>
===DONE===
--EXPECTF--
Warning: SplPriorityQueue::insert() expects exactly 2 parameters, 0 given in %s on line 7
NULL

Warning: SplPriorityQueue::insert() expects exactly 2 parameters, 1 given in %s on line 8
NULL
bool(true)

Warning: SplPriorityQueue::insert() expects exactly 2 parameters, 3 given in %s on line 10
NULL
===DONE===
