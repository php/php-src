--TEST--
DBM FirstKey/NextKey Loop Test With 5 Items
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
	$a = dbmfirstkey("./test.dbm");
	$i=0;
	while($a) {
		$a = dbmnextkey("./test.dbm",$a);
		$i++;
	}
	dbmclose("./test.dbm");
	echo $i
?>
--EXPECT--
5
