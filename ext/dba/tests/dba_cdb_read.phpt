--TEST--
DBA CDB handler test (read only)
--SKIPIF--
<?php 
	if (!extension_loaded('dba')) die('skip dba extension not available');
	if (!in_array('cdb', dba_handlers())) die('skip CDB handler not available');
?>
--FILE--
<?php
	echo "database handler: cdb\n";
	$handler = 'cdb';
	$db_file = dirname(__FILE__).'/test.cdb';
	if (($db_file=dba_open($db_file, "r", $handler))!==FALSE) {
		$a = dba_firstkey($db_file);
		$i=0;
		echo "?$a";
		while($a) {
			$a = dba_nextkey($db_file);
			echo $a;
			$i++;
		}
		echo "\n";
		echo $i;
		for ($i=1; $i<5; $i++) {
			echo dba_exists($i, $db_file) ? "Y" : "N";
		}
		echo "\n=";
		echo dba_fetch(1, $db_file);
		echo dba_fetch(2, $db_file);
		echo dba_fetch(3, $db_file);
		echo dba_fetch(4, $db_file);
		dba_close($db_file);
	} else {
		echo "Error creating database\n";
	}
?>
--EXPECT--
database handler: cdb
?1212314
7YYYY
=1234