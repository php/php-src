--TEST--
PostgreSQL pg_insert()
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
include("pg_insert.inc");
?>
--EXPECT--
INSERT INTO php_pgsql_test (num,str,bin) VALUES (1234,'AAA','BBB');
Ok
