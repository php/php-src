--TEST--
pg_trace
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

include('config.inc');

$db = pg_connect($conn_str);
$tracefile = __DIR__ . '/trace.tmp';

var_dump(pg_trace($tracefile, 'w', $db));
$res = pg_query($db, 'select 1');

?>
--EXPECTF--
bool(true)
