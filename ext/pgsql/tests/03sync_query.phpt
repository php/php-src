--TEST--
PostgreSQL sync query
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');
$table_name = "table_03sync_query";

$db = pg_connect($conn_str);
pg_query($db, "CREATE TABLE {$table_name} (num int, str text, bin bytea)");
pg_query($db, "INSERT INTO {$table_name} DEFAULT VALUES");

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

try {
    pg_fetch_result($result, 0, -1);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    pg_fetch_result($result, 0, 3);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    pg_fetch_result($result, 0, "unknown");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    pg_fetch_all_columns($result, -1);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    pg_fetch_all_columns($result, 3);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
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
pg_field_num($result, "num");
pg_field_size($result, 0);
pg_field_type($result, 0);
pg_field_prtlen($result, 0);
pg_field_is_null($result, 0);

try {
    pg_field_is_null($result, -1);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    pg_field_is_null($result, 3);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    pg_field_is_null($result, "unknown");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    pg_field_name($result, -1);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    pg_field_name($result, 3);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    pg_field_table($result, -1);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    pg_field_table($result, 3);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

$result = pg_query($db, "INSERT INTO ".$table_name." VALUES (9999, 'ABC');");
pg_last_oid($result);
var_dump(pg_fetch_all($result));

pg_free_result($result);
pg_close($db);

echo "OK";
?>
--CLEAN--
<?php
include('inc/config.inc');
$table_name = "table_03sync_query";

$db = pg_connect($conn_str);
pg_query($db, "DROP TABLE IF EXISTS {$table_name}");
?>
--EXPECT--
Argument #3 must be greater than or equal to 0
Argument #3 must be less than the number of fields for this result set
Argument #3 must be a field name from this result set
pg_fetch_all_columns(): Argument #2 ($field) must be greater than or equal to 0
pg_fetch_all_columns(): Argument #2 ($field) must be less than the number of fields for this result set
Argument #2 must be greater than or equal to 0
Argument #2 must be less than the number of fields for this result set
Argument #2 must be a field name from this result set
pg_field_name(): Argument #2 ($field) must be greater than or equal to 0
pg_field_name(): Argument #2 ($field) must be less than the number of fields for this result set
pg_field_table(): Argument #2 ($field) must be greater than or equal to 0
pg_field_table(): Argument #2 ($field) must be less than the number of fields for this result set
array(0) {
}
OK
