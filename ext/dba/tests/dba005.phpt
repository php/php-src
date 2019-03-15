--TEST--
DBA FirstKey/NextKey Loop Test With 5 Items
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
		dba_insert("key1", "Content String 1", $db_file);
		dba_insert("key2", "Content String 2", $db_file);
		dba_insert("key3", "Third Content String", $db_file);
		dba_insert("key4", "Another Content String", $db_file);
		dba_insert("key5", "The last content string", $db_file);
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
--CLEAN--
<?php
	require(__DIR__ .'/clean.inc');
?>
--EXPECTF--
database handler: %s
5YYYYY
