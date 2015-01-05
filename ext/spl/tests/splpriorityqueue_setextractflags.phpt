--TEST--
SPL: splpriorityqueue setExtractFlags() Test arguments
--CREDITS--
Roshan Abraham (roshanabrahams@gmail.com)
TestFest London May 2009
--FILE--
<?php

$sp = new SplPriorityQueue();

$sp->setExtractFlags(1,1); // Should throw a warning as setExtractFlags expects only 1 argument

?>
--EXPECTF--

Warning: SplPriorityQueue::setExtractFlags() expects exactly 1 parameter, 2 given in %s on line %d

