--TEST--
PostgreSQL sync query params
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
include("skipif.inc");
if (!function_exists('pg_query_params')) die('skip function pg_query_params() does not exist');
?>
--FILE--
<?php

include('config.inc');

$db = pg_connect($conn_str);
$result = pg_query_params($db, "SELECT * FROM ".$table_name." WHERE num > \$1;", array(100));
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
	pg_fetch_object($result);
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
pg_num_rows(pg_query_params($db, "SELECT * FROM ".$table_name." WHERE num > \$1;", array(100)));
pg_num_fields(pg_query_params($db, "SELECT * FROM ".$table_name." WHERE num > \$1;", array(100)));
pg_field_name($result, 0);
pg_field_num($result, $field_name);
pg_field_size($result, 0);
pg_field_type($result, 0);
pg_field_prtlen($result, null, 0);
pg_field_is_null($result, null, 0);

$result = pg_query_params($db, "INSERT INTO ".$table_name." VALUES (\$1, \$2);", array(9999, "A'BC"));
pg_last_oid($result);

pg_free_result($result);
pg_close($db);

echo "OK";
?>
--EXPECT--
OK
