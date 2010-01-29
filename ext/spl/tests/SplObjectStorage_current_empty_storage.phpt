--TEST--
Check that SplObjectStorage::current returns NULL when storage is empty
--CREDITS--
PHPNW Testfest 2009 - Simon Westcott (swestcott@gmail.com)
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php

$s = new SplObjectStorage();

var_dump($s->current());

?>
--EXPECT--
NULL

