--TEST--
SPL: splpriorityqueue extract() Test arguments
--CREDITS--
Roshan Abraham (roshanabrahams@gmail.com)
TestFest London May 2009
--FILE--
<?php

$sp = new SplPriorityQueue();

$sp->insert("1",1);

$sp->extract(1); // Should throw a warning as extract expects NO arguments

?>
--EXPECTF--

Warning: SplPriorityQueue::extract() expects exactly 0 parameters, 1 given in %s on line %d

