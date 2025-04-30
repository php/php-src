--TEST--
PostgreSQL pg_close_stmt
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc");
if (!function_exists("pg_close_stmt")) die("skip pg_close_stmt unsupported");
?>
--FILE--
<?php
include('inc/config.inc');


$query = 'SELECT $1::text IS NULL;';
$params_null = [null];

$db = pg_connect($conn_str);
$res = pg_prepare($db, 'test', $query);

$res = pg_execute($db, 'test', $params_null);
try {
	pg_close_stmt($db, '');
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
$res = pg_close_stmt($db, 'test');
var_dump($res !== false);
var_dump(pg_result_status($res) === PGSQL_COMMAND_OK);
pg_prepare($db, 'test', $query);
$res = pg_execute($db, 'test', $params_null);
pg_free_result($res);

pg_close($db);

?>
--EXPECT--
pg_close_stmt(): Argument #2 ($statement_name) must not be empty
bool(true)
bool(true)
