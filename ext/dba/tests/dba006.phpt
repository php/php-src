--TEST--
DBA FirstKey/NextKey with 2 deletes
--SKIPIF--
<?php 
	require_once('skipif.inc');
?>
--FILE--
<?php
	require_once('test.inc');
	echo "database handler: $handler\n";
	if (($db_file=dba_open($db_file, "n", $handler))!==FALSE) {
		dba_insert("key1", "Content String 1", $db_file);
		dba_insert("key2", "Content String 2", $db_file);
		dba_insert("key3", "Third Content String", $db_file);
		dba_insert("key4", "Another Content String", $db_file);
		dba_insert("key5", "The last content string", $db_file);
		dba_delete("key3", $db_file);
		dba_delete("key1", $db_file);
		$a = dba_firstkey($db_file);
		$i=0;
		while($a) {
			$a = dba_nextkey($db_file);
			$i++;
		}
		echo $i;
		for ($i=1; $i<6; $i++) {
			echo dba_exists("key$i", $db_file) ? "Y" : "N";
		}
		dba_close($db_file);
	} else {
		echo "Error creating database\n";
	}
?>
--EXPECTF--
database handler: %s
3NYNYY