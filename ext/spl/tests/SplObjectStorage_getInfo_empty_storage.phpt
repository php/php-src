--TEST--
Check that SplObjectStorage::getInfo returns NULL when storage is empty
--CREDITS--
PHPNW Testfest 2009 - Simon Westcott (swestcott@gmail.com)
--FILE--
<?php

$s = new SplObjectStorage();

var_dump($s->getInfo());

?>
--EXPECT--
NULL

