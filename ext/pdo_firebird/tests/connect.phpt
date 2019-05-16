--TEST--
PDO_Firebird: connect/disconnect
--SKIPIF--
<?php require('skipif.inc'); ?>
--FILE--
<?php
	require("testdb.inc");

	unset($dbh);
	echo "done\n";

?>
--EXPECT--
done
