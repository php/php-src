--TEST--
PostgreSQL escape functions
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
include("escape.inc");
?>
--EXPECT--
pg_escape_string() is NOT Ok
string(9) "ABC\ABC\'"
string(12) "ABC\\ABC\\''"
string(10) "ABC\\ABC\'"
pg_escape_bytea() is Ok
