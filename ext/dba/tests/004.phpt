--TEST--
DBA Multiple Insert/Fetch Test
--SKIPIF--
<?php 
	require_once('skipif.inc');
?>
--FILE--
<?php
	require_once('test.inc');
	if (($db_file=dba_open($db_file, "n", $handler))!==FALSE) {
		dba_insert("key1", "Content String 1", $db_file);
		dba_insert("key2", "Content String 2", $db_file);
		dba_insert("key3", "Third Content String", $db_file);
		dba_insert("key4", "Another Content String", $db_file);
		dba_insert("key5", "The last content string", $db_file);
		$a = dba_fetch("key4", $db_file);
		$b = dba_fetch("key2", $db_file);
		dba_close($db_file);
		echo "$a $b";
	} else {
		echo "Error creating database\n";
	}
?>
--EXPECT--
Another Content String Content String 2
