--TEST--
PostgreSQL pg_update()
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
include("pg_update.inc");
?>
--EXPECT--
UPDATE php_pgsql_test SET num=1234,str='ABC',bin='XYZ' WHERE num=1234;
Ok
