--TEST--
pg_close() default link after connection variable has been dropped
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
?>
--EXPECTF--
pg_close() expects exactly 1 argument, 0 given
