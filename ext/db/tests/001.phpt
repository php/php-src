--TEST--
DBM File Creation Test
--POST--
--GET--
--FILE--
<?php
	dbmopen("./test.dbm","n");
	dbmclose("./test.dbm");
?>
--EXPECT--

