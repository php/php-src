--TEST--
DBM FirstKey/NextKey with 2 deletes
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
		dbmdelete($db_file, "key3");
		dbmdelete($db_file, "key1");
		$a = dbmfirstkey($db_file);
		$i=0;
		while($a) {
			$a = dbmnextkey($db_file, $a);
			$i++;
		}
		dbmclose($db_file);
		echo $i;
	} else {
		echo "Error creating database\n";
	}
?>
--EXPECT--
3
