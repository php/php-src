<?php
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
		echo "\n";
		echo dba_fetch("key2", $db_file)."\n";
		dba_replace("key2", "Content 2 replaced", $db_file);
		echo dba_fetch("key2", $db_file)."\n";
		dba_close($db_file);
	} else {
		echo "Error creating database\n";
	}
?>