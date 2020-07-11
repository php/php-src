--TEST--
Check that SplObjectStorage::current returns NULL when storage is empty
--CREDITS--
PHPNW Testfest 2009 - Simon Westcott (swestcott@gmail.com)
--FILE--
<?php

$s = new SplObjectStorage();

var_dump($s->current());

?>
--EXPECT--
NULL
