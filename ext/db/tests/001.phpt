--TEST--
DBM File Creation Test
--SKIPIF--
<?php if (!extension_loaded("db")) print "skip"; ?>
--FILE--
<?php
	require_once('test.inc');
	if (dbmopen($db_file, "n") && file_exists($db_file)) {
    	echo "database file created\n";
	} else {
    	echo "$db_file does not exist\n";
    }
	dbmclose($db_file);
?>
--EXPECT--
database file created