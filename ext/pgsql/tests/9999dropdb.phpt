--TEST--
PostgreSQL drop db
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
include("dropdb.inc");
?>
--EXPECT--
OK
