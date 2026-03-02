--TEST--
pg_result_memory_size
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
include("inc/skipif.inc");
if (!function_exists('pg_result_memory_size')) die('skip function pg_result_memory_size() does not exist');
?>
--FILE--
<?php
include('inc/config.inc');

$db = pg_connect($conn_str);

$result = pg_query($db, 'select 1');
$size_1 = pg_result_memory_size($result);

$result = pg_query($db, "select generate_series(1, 10000) as i, repeat('string', 100)");
$size_2 = pg_result_memory_size($result);

var_dump($size_1);
var_dump($size_2);
var_dump($size_1 < $size_2);
?>
--EXPECTF--
int(%d)
int(%d)
bool(true)
