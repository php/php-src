--TEST--
SPL: SplHeap current, check looping through an empty heap gives you no values
--CREDITS--
Mark Schaschke (mark@fractalturtle.com)
TestFest London May 2009
--FILE--
<?php
$h = new SplMinHeap();

foreach ($h as $val) { echo 'FAIL'; }
?>
--EXPECT--
