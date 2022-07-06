--TEST--
PostgreSQL copy functions
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

include('config.inc');

$db = pg_connect($conn_str);

$rows = pg_copy_to($db, $table_name);

pg_query($db, "DELETE FROM $table_name");

pg_copy_from($db, $table_name, $rows);

echo "OK";

?>
--EXPECT--
OK
