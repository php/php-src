--TEST--
DBM Insert/Replace/Fetch Test
--SKIPIF--
<?php if (!extension_loaded("db")) print "skip"; ?>
--FILE--
<?php
	require_once('test.inc');
	if (dbmopen($db_file, "n")) {
		dbminsert($db_file, "key1", "This is a test insert");
		dbmreplace($db_file, "key1", "This is the replacement text");
		@dbminsert($db_file, "key1", "This is another replacement text?");
		$a = dbmfetch($db_file, "key1");
		dbmclose($db_file);
		echo $a;
	} else {
		echo "Error creating database\n";
	}
?>
--EXPECT--
This is the replacement text
