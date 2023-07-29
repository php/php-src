--TEST--
Reopen connection after it was closed
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
include('config.inc');

/* Run me under valgrind */
$db1 = pg_connect($conn_str);
unset($db1);
try {
    pg_close();
} catch (ArgumentCountError $e) {
    echo $e->getMessage() . PHP_EOL;
}

$db2 = pg_connect($conn_str);
unset($db2);
try {
    pg_close();
} catch (ArgumentCountError $e) {
    echo $e->getMessage() . PHP_EOL;
}
?>
--EXPECTF--
pg_close() expects exactly 1 argument, 0 given
pg_close() expects exactly 1 argument, 0 given
