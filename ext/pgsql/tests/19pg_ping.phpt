--TEST--
PostgreSQL pg_ping() functions
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
include("pg_ping.inc");
?>
--EXPECT--
bool(true)
