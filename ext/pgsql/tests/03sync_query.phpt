--TEST--
PostgreSQL sync query
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

include('config.inc');

$db = pg_connect($conn_str);

$result = pg_query($db, "SELECT * FROM ".$table_name.";");
if (!($rows   = pg_num_rows($result)))
{
	echo "pg_num_row() error\n";
}
for ($i=0; $i < $rows; $i++) 
{
	pg_fetch_array($result, $i, PGSQL_NUM);
}
for ($i=0; $i < $rows; $i++) 
{
	pg_fetch_object($result, $i, PGSQL_ASSOC);
}
for ($i=0; $i < $rows; $i++) 
{
	pg_fetch_row($result, $i);
}
for ($i=0; $i < $rows; $i++) 
{
	pg_fetch_result($result, $i, 0);
}

pg_result_error($result);
pg_num_rows(pg_query($db, "SELECT * FROM ".$table_name.";"));
pg_num_fields(pg_query($db, "SELECT * FROM ".$table_name.";"));
pg_field_name($result, 0);
pg_field_num($result, $field_name);
pg_field_size($result, 0);
pg_field_type($result, 0);
pg_field_prtlen($result, 0);
pg_field_is_null($result, 0);

$result = pg_query($db, "INSERT INTO ".$table_name." VALUES (9999, 'ABC');");
pg_last_oid($result);

pg_free_result($result);
pg_close($db);

echo "OK";
?>
--EXPECT--
OK
