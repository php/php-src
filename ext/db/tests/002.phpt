--TEST--
DBM Insert/Fetch Test
--POST--
--GET--
--FILE--
<?php
	dbmopen("./test.dbm","n");
	dbminsert("./test.dbm","key1","This is a test insert");
	$a = dbmfetch("./test.dbm","key1");
	dbmclose("./test.dbm");
	echo $a
?>
--EXPECT--
This is a test insert
