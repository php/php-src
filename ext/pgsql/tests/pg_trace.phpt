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

try {
	pg_trace($tracefile, 'w', $db, 56432);
} catch (ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
var_dump(pg_trace($tracefile, 'w', $db, 0));
$res = pg_query($db, 'select 1');

?>
--EXPECT--
pg_trace(): Argument #4 ($trace_mode) cannot set as trace is unsupported
bool(true)
