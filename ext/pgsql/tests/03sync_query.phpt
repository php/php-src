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
if (function_exists('pg_result_error_field')) {
	pg_result_error_field($result, PGSQL_DIAG_SEVERITY);
	pg_result_error_field($result, PGSQL_DIAG_SQLSTATE);
	pg_result_error_field($result, PGSQL_DIAG_MESSAGE_PRIMARY);
	pg_result_error_field($result, PGSQL_DIAG_MESSAGE_DETAIL);
	pg_result_error_field($result, PGSQL_DIAG_MESSAGE_HINT);
	pg_result_error_field($result, PGSQL_DIAG_STATEMENT_POSITION);
	if (defined('PGSQL_DIAG_INTERNAL_POSITION'))
	{
		pg_result_error_field($result, PGSQL_DIAG_INTERNAL_POSITION);
	}
	if (defined('PGSQL_DIAG_INTERNAL_QUERY'))
	{
		pg_result_error_field($result, PGSQL_DIAG_INTERNAL_QUERY);
	}
	pg_result_error_field($result, PGSQL_DIAG_CONTEXT);
	pg_result_error_field($result, PGSQL_DIAG_SOURCE_FILE);
	pg_result_error_field($result, PGSQL_DIAG_SOURCE_LINE);
	pg_result_error_field($result, PGSQL_DIAG_SOURCE_FUNCTION);
}
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
