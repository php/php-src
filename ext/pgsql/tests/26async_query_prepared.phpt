--TEST--
PostgreSQL async prepared queries
--SKIPIF--
<?php
include("skipif.inc");
if (!function_exists('pg_send_prepare')) die('skip function pg_send_prepare() does not exist');
?>
--FILE--
<?php

include('config.inc');

$db = pg_connect($conn_str);

$version = pg_version($db);
if ($version['protocol'] >= 3) {
	if (!pg_send_prepare($db, 'php_test', "SELECT * FROM ".$table_name." WHERE num > \$1;")) {
		echo "pg_send_prepare() error\n";
	}
	while(pg_connection_busy($db));  // busy wait: intended
	if (pg_connection_status($db) === PGSQL_CONNECTION_BAD) {
		echo "pg_connection_status() error\n";
	}
	if (!($result = pg_get_result($db)))
	{
		echo "pg_get_result() error\n";
	}
	pg_free_result($result);

	if (!pg_send_execute($db, 'php_test', array(100))) {
		echo "pg_send_execute() error\n";
	}
	while(pg_connection_busy($db));  // busy wait: intended
	if (pg_connection_status($db) === PGSQL_CONNECTION_BAD) {
		echo "pg_connection_status() error\n";
	}
	if (!($result = pg_get_result($db)))
	{
		echo "pg_get_result() error\n";
	}

	if (!($rows = pg_num_rows($result))) {
		echo "pg_num_rows() error\n";
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

	pg_num_rows(pg_query_params($db, "SELECT * FROM ".$table_name." WHERE num > \$1;", array(100)));
	pg_num_fields(pg_query_params($db, "SELECT * FROM ".$table_name." WHERE num > \$1;", array(100)));
	pg_field_name($result, 0);
	pg_field_num($result, $field_name);
	pg_field_size($result, 0);
	pg_field_type($result, 0);
	pg_field_prtlen($result, 0);
	pg_field_is_null($result, 0);

	if (!pg_send_prepare($db, "php_test2", "INSERT INTO ".$table_name." VALUES (\$1, \$2);"))
	{
		echo "pg_send_prepare() error\n";
	}
	while(pg_connection_busy($db));  // busy wait: intended
	if (pg_connection_status($db) === PGSQL_CONNECTION_BAD) {
		echo "pg_connection_status() error\n";
	}
	if (!($result = pg_get_result($db)))
	{
		echo "pg_get_result() error\n";
	}
	pg_free_result($result);

	if (!pg_send_execute($db, "php_test2", array(9999, "A'BC")))
	{
		echo "pg_send_execute() error\n";
	}
	while(pg_connection_busy($db));  // busy wait: intended
	if (pg_connection_status($db) === PGSQL_CONNECTION_BAD) {
		echo "pg_connection_status() error\n";
	}
	if (!($result = pg_get_result($db)))
	{
		echo "pg_get_result() error\n";
	}

	pg_last_oid($result);
	pg_free_result($result);
}
pg_close($db);

echo "OK";
?>
--EXPECT--
OK
