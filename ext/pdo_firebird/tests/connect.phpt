--TEST--
PDO_Firebird: connect/disconnect
--SKIPIF--
<?php include("skipif.inc"); ?>
<?php function_exists("ibase_query") or die("skip"); ?>
--FILE--
<?php /* $Id$ */

	require("testdb.inc");

	$db = new PDO("firebird:dbname=$test_base",$user,$password) or die;
	unset($db);
	echo "done\n";

?>
--EXPECT--
done
