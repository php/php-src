--TEST--
DBA Insert/Replace/Fetch Test
--SKIPIF--
<?php 
	require_once('skipif.inc');
?>
--FILE--
<?php
	require_once('test.inc');
	if (($db_file=dba_open($db_file, "n", $handler))!==FALSE) {
		dba_insert("key1", "This is a test insert", $db_file);
		dba_replace("key1", "This is the replacement text", $db_file);
		$a = dba_fetch("key1", $db_file);
		dba_close($db_file);
		echo $a;
	} else {
		echo "Error creating database\n";
	}
?>
--EXPECT--
This is the replacement text
