--TEST--
DBM Multiple Insert/Fetch Test
--SKIPIF--
<?php if (!extension_loaded("db")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php
	dbmopen("./test.dbm","n");
	dbminsert("./test.dbm","key1","Content String 1");
	dbminsert("./test.dbm","key2","Content String 2");
	dbminsert("./test.dbm","key3","Third Content String");
	dbminsert("./test.dbm","key4","Another Content String");
	dbminsert("./test.dbm","key5","The last content string");
	$a = dbmfetch("./test.dbm","key4");
	$b = dbmfetch("./test.dbm","key2");
	dbmclose("./test.dbm");
	echo "$a $b";
?>
--EXPECT--
Another Content String Content String 2
