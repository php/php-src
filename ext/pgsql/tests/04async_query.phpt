--TEST--
PostgreSQL async query
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
include("async_query.inc");
?>
--EXPECT--
OK
