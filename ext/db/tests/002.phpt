--TEST--
DBM Insert/Fetch Test
--SKIPIF--
<?php if (!extension_loaded("db")) print "skip";?>
--FILE--
<?php
	require_once('test.inc');
	if (dbmopen($db_file, "n")) {
		dbminsert($db_file, "key1", "This is a test insert");
		echo dbmfetch($db_file, "key1");
		dbmclose($db_file);
	} else {
		echo "Error creating database\n";
	}
?>
--EXPECT--
This is a test insert
