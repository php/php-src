--TEST--
SPL: SplPriorityQueue: test compare
--CREDITS--
Mark Schaschke (mark@fractalturtle.com)
TestFest London May 2009
--FILE--
<?php
$h = new SplPriorityQueue();
var_dump($h->compare(4, 5) < 0);
var_dump($h->compare(5, 5) == 0);
var_dump($h->compare(5, 4) > 0);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
