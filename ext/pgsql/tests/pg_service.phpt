--TEST--
PostgreSQL connection service field support
--EXTENSIONS--
pgsql
--SKIPIF--
<?php 
include("inc/skipif.inc"); 
if (!function_exists("pg_service")) die("skip pg_service unsupported");
?>
--FILE--
<?php
include('inc/config.inc');

$db = pg_connect($conn_str);
var_dump(pg_service($db));
pg_close($db);
?>
--EXPECTF--
string(%d) "%A"
