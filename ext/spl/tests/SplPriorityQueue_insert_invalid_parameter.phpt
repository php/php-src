--TEST--
Check that SplPriorityQueue::insert generate a warning and returns NULL when rubbish params are passed
--CREDITS--
PHPNW Testfest 2009 - Simon Westcott (swestcott@gmail.com)
--FILE--
<?php

$h = new SplPriorityQueue();

var_dump($h->insert(NULL));

?>
--EXPECTF--
Warning: SplPriorityQueue::insert() expects exactly 2 parameters, 1 given in %s on line %d
NULL

