--TEST--
DBM File Creation Test
--SKIPIF--
<?php if (!extension_loaded("db")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php
	dbmopen("./test.dbm","n");
	dbmclose("./test.dbm");
?>
--EXPECT--

