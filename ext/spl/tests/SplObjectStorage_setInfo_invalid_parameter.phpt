--TEST--
Check that SplObjectStorage::setInfo returns NULL when no param is passed
--CREDITS--
PHPNW Testfest 2009 - Simon Westcott (swestcott@gmail.com)
--FILE--
<?php

$s = new SplObjectStorage();

var_dump($s->setInfo());

?>
--EXPECTF--
Warning: SplObjectStorage::setInfo() expects exactly 1 parameter, 0 given in %s on line %d
NULL
