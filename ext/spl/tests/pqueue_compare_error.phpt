--TEST--
SPL: Priority queue compare, illegal number of args
--CREDITS--
Mark Schaschke (mark@fractalturtle.com)
TestFest London May 2009
--FILE--
<?php
$h = new SplPriorityQueue();
$h->compare();
$h->compare(1);
$h->compare(1, 2, 3);
?>
--EXPECTF--
Warning: SplPriorityQueue::compare() expects exactly 2 parameters, 0 given in %s

Warning: SplPriorityQueue::compare() expects exactly 2 parameters, 1 given in %s

Warning: SplPriorityQueue::compare() expects exactly 2 parameters, 3 given in %s
