--TEST--
DBA File Creation Test
--SKIPIF--
<?php 
	require_once('skipif.inc');
	die("info $HND handler used");
?>
--FILE--
<?php
	require_once('test.inc');
	echo "database handler: $handler\n";
	if (($db_file=dba_open($db_file, "n", $handler))!==FALSE) {
    	echo "database file created\n";
		dba_close($db_file);
	} else {
    	echo "$db_file does not exist\n";
    }
?>
--EXPECTF--
database handler: %s
database file created