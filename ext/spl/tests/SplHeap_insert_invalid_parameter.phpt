--TEST--
Check that SplHeap::insert generate a warning and returns NULL when $value is missing
--CREDITS--
PHPNW Testfest 2009 - Simon Westcott (swestcott@gmail.com)
--FILE--
<?php

$h = new SplMaxHeap();

var_dump($h->insert());

?>
--EXPECTF--
Warning: SplHeap::insert() expects exactly 1 parameter, 0 given in %s on line %d
NULL

