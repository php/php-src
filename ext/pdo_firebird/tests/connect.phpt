--TEST--
PDO_Firebird: connect/disconnect
--SKIPIF--
<?php if (!extension_loaded('pdo_firebird')) die('skip'); ?>
--FILE--
<?php
	require("testdb.inc");

	unset($dbh);
	echo "done\n";

?>
--EXPECT--
done
