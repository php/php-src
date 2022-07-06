--TEST--
pg_close() default link after connection variable has been dropped
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
include('config.inc');

/* Run me under valgrind */
$db1 = pg_connect($conn_str);
unset($db1);
var_dump(pg_close());
?>
--EXPECT--
bool(true)
