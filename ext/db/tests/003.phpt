--TEST--
DBM Insert/Replace/Fetch Test
--SKIPIF--
<?php if (!extension_loaded("db")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php
	dbmopen("./test.dbm","n");
	dbminsert("./test.dbm","key1","This is a test insert");
	dbmreplace("./test.dbm","key1","This is the replacement text");
	$a = dbmfetch("./test.dbm","key1");
	dbmclose("./test.dbm");
	echo $a
?>
--EXPECT--
This is the replacement text
