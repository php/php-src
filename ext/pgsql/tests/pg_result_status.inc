<?php
include 'config.inc';

$db = pg_connect($conn_str);

$sql = "SELECT * FROM ".$table_name." WHERE num = -2";
$result = pg_query($db, "BEGIN;END");

echo pg_result_status($result)."\n";
echo pg_result_status($result, PGSQL_STATUS_STRING)."\n";


?>