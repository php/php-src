--TEST--
Check that SplHeap::isEmpty standard success test
--CREDITS--
PHPNW Testfest 2009 - Simon Westcott (swestcott@gmail.com)
--FILE--
<?php

$h = new SplMaxHeap();

var_dump($h->isEmpty());

?>
--EXPECTF--
bool(true)
