--TEST--
PostgreSQL escape functions
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
include("escape.inc");
?>
--EXPECT--
pg_escape_string() is Ok
pg_escape_bytea() is Ok
pg_escape_bytea() actually works with databse
