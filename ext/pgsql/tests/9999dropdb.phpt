--TEST--
PostgreSQL drop db
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
// drop test table

include('config.inc');

$db = pg_connect($conn_str);
pg_query($db, "DROP VIEW {$view_name}");
pg_query($db, "DROP TABLE ".$table_name);
@pg_query($db, "DROP TABLE ".$table_name_92);

echo "OK";

?>
--EXPECT--
OK
