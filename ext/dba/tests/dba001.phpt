--TEST--
DBA File Creation Test
--SKIPIF--
<?php 
	require_once('skipif.inc');
?>
--FILE--
<?php
	require_once('test.inc');
	if (($db_file=dba_open($db_file, "n", $handler))!==FALSE) {
    	echo "database file created with $handler.\n";
	} else {
    	echo "$db_file does not exist\n";
    }
	dba_close($db_file);
?>
--EXPECTF--
database file created with %s.