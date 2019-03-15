--TEST--
DBA Insert/Replace/Fetch Test
--SKIPIF--
<?php
	require_once(__DIR__ .'/skipif.inc');
	die("info $HND handler used");
?>
--FILE--
<?php
	require_once(__DIR__ .'/test.inc');
	echo "database handler: $handler\n";
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
--CLEAN--
<?php
	require(__DIR__ .'/clean.inc');
?>
--EXPECTF--
database handler: %s
This is the replacement text
