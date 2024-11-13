--TEST--
GH-13354 (null-by-reference handling in pg_execute, pg_send_query_params, pg_send_execute)
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php
include('inc/config.inc');

$db = pg_connect($conn_str);
$val = null;

$query = 'SELECT $1::text IS NULL;';
$params_null = [null];
$params_null_by_ref = [&$val];

pg_prepare($db, 'test', $query);


// method 1, pg_execute
$val = null;
$res = pg_execute($db, 'test', $params_null);
echo "pg_execute, null value: " . pg_fetch_result($res, 0, 0) . "\n";
pg_free_result($res);

$res = pg_execute($db, 'test', $params_null_by_ref);
echo "pg_execute, null value by reference: " . pg_fetch_result($res, 0, 0) . "\n";
pg_free_result($res);


// method 2, pg_query_params
$res = pg_query_params($db, $query, $params_null);
echo "pg_query_params, null value: " . pg_fetch_result($res, 0, 0) . "\n";
pg_free_result($res);

$res = pg_query_params($db, $query, $params_null_by_ref);
echo "pg_query_params, null value by reference: " . pg_fetch_result($res, 0, 0) . "\n";
pg_free_result($res);


// method 3, pg_send_query_params
$res = pg_send_query_params($db, $query, $params_null);
pg_consume_input($db);
$res = pg_get_result($db);
echo "pg_send_query_params, null value: " . pg_fetch_result($res, 0, 0) . "\n";
pg_free_result($res);

$res = pg_send_query_params($db, $query, $params_null_by_ref);
pg_consume_input($db);
$res = pg_get_result($db);
echo "pg_send_query_params, null value by reference: " . pg_fetch_result($res, 0, 0) . "\n";
pg_free_result($res);


// method 4, pg_send_prepare, pg_send_execute
pg_send_execute($db, 'test', $params_null);
pg_consume_input($db);
$res = pg_get_result($db);
echo "pg_send_execute, null value: " . pg_fetch_result($res, 0, 0) . "\n";
pg_free_result($res);

pg_send_execute($db, 'test', $params_null_by_ref);
pg_consume_input($db);
$res = pg_get_result($db);
echo "pg_send_execute, null value by reference: " . pg_fetch_result($res, 0, 0) . "\n";
pg_free_result($res);

pg_close($db);

?>
--EXPECT--
pg_execute, null value: t
pg_execute, null value by reference: t
pg_query_params, null value: t
pg_query_params, null value by reference: t
pg_send_query_params, null value: t
pg_send_query_params, null value by reference: t
pg_send_execute, null value: t
pg_send_execute, null value by reference: t
