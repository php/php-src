--TEST--
DBA magic_quotes_runtime Test
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
		ini_set('magic_quotes_runtime', 0);
		dba_insert("key1", '"', $db_file);
		var_dump(dba_fetch("key1", $db_file));
		ini_set('magic_quotes_runtime', 1);
		var_dump(dba_fetch("key1", $db_file));
		dba_replace("key1", '\"', $db_file);
		var_dump(dba_fetch("key1", $db_file));
		ini_set('magic_quotes_runtime', 0);
		var_dump(dba_fetch("key1", $db_file));
		dba_close($db_file);
	} else {
		echo "Error creating database\n";
	}
?>
--EXPECTF--
database handler: %s
string(1) """
string(2) "\""
string(2) "\""
string(1) """
