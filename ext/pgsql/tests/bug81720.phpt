--TEST--
Bug #81720 (Uninitialized array in pg_query_params() leading to RCE)
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
include('config.inc');

$conn = pg_connect($conn_str);

try {
    pg_query_params($conn, 'SELECT $1, $2', [1, new stdClass()]);
} catch (Throwable $ex) {
    echo $ex->getMessage(), PHP_EOL;
}

try {
    pg_send_prepare($conn, "my_query", 'SELECT $1, $2');
    pg_get_result($conn);
    pg_send_execute($conn, "my_query", [1, new stdClass()]);
} catch (Throwable $ex) {
    echo $ex->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Object of class stdClass could not be converted to string
Object of class stdClass could not be converted to string
