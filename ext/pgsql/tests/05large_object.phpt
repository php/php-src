--TEST--
PostgreSQL large object
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
include("large_object.inc");
?>
--EXPECT--
large object data
OK
