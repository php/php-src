--TEST--
PostgreSQL pg_result_status()
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
include("pg_result_status.inc");
?>
--EXPECT--
1
COMMIT
