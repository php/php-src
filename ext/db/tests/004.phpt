--TEST--
DBM Multiple Insert/Fetch Test
--SKIPIF--
<?php if (!extension_loaded("db")) print "skip"; ?>
--FILE--
<?php
	require_once('test.inc');
	if (dbmopen($db_file, "n")) {
		dbminsert($db_file, "key1", "Content String 1");
		dbminsert($db_file, "key2", "Content String 2");
		dbminsert($db_file, "key3", "Third Content String");
		dbminsert($db_file, "key4", "Another Content String");
		dbminsert($db_file, "key5", "The last content string");
		$a = dbmfetch($db_file, "key4");
		$b = dbmfetch($db_file, "key2");
		dbmclose($db_file);
		echo "$a $b";
	} else {
		echo "Error creating database\n";
	}
?>
--EXPECT--
Another Content String Content String 2
