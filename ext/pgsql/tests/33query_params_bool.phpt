--TEST--
PostgreSQL prepared queries with bool constants
--SKIPIF--
<?php
include("skipif.inc");
if (!function_exists('pg_prepare')) die('skip function pg_prepare() does not exist');
?>
--FILE--
<?php

include('config.inc');

$db = pg_connect($conn_str);

$version = pg_version($db);
if ($version['protocol'] >= 3) {
    $result = pg_query_params($db, "SELECT * FROM ".$table_name." WHERE num >= $1;", array(true));
    // bug occurs with false as it turns out as empty.
    $result = pg_query_params($db, "SELECT * FROM ".$table_name." WHERE num <> $1;", array(false));
    pg_free_result($result);
}
pg_close($db);

echo "OK";
?>
--EXPECT--
OK

