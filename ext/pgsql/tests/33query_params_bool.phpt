--TEST--
PostgreSQL pg_query_params bool parameter rejection
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');

$db = pg_connect($conn_str);

try {
    pg_query_params($db, "SELECT * FROM ".$table_name." WHERE num >= $1;", array(true));
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    pg_query_params($db, "SELECT * FROM ".$table_name." WHERE num <> $1;", array(false));
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

pg_close($db);

echo "OK";
?>
--EXPECT--
pg_query_params(): Argument #3 ($params) must not contain boolean values, use a string representation instead
pg_query_params(): Argument #3 ($params) must not contain boolean values, use a string representation instead
OK
