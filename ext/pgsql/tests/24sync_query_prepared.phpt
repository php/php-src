--TEST--
PostgreSQL sync prepared queries
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
include("inc/skipif.inc");
if (!function_exists('pg_prepare')) die('skip function pg_prepare() does not exist');
?>
--FILE--
<?php

include('inc/config.inc');

$db = pg_connect($conn_str);

$result = pg_prepare($db, "php_test", "SELECT * FROM ".$table_name." WHERE num > \$1;");
pg_result_error($result);
pg_free_result($result);
$result = pg_execute($db, "php_test", array(100));
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
pg_num_rows(pg_execute($db, "php_test", array(100)));
pg_num_fields(pg_execute($db, "php_test", array(100)));
pg_field_name($result, 0);
pg_field_num($result, $field_name);
pg_field_size($result, 0);
pg_field_type($result, 0);
pg_field_prtlen($result, 0);
pg_field_is_null($result, 0);

$result = pg_prepare($db, "php_test2", "INSERT INTO ".$table_name." VALUES (\$1, \$2);");
pg_result_error($result);
pg_free_result($result);
$result = pg_execute($db, "php_test2", array(9999, "A'BC"));
pg_last_oid($result);

pg_free_result($result);
pg_close($db);

echo "OK";
?>
--EXPECT--
OK
