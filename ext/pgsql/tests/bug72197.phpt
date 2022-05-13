--TEST--
Bug #72197 pg_lo_create arbitrary read
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
/* This shouldn't crash. */
$var1=-32768;
$var2="12";
try {
    pg_lo_create($var1, $var2);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

/* This should work correctly. */
include('config.inc');

/* Check with explicit link. */
$conn = pg_connect($conn_str);
pg_query($conn, "BEGIN");
$oid = pg_lo_create($conn);
var_dump($oid);

/* Check with default link */
$oid = pg_lo_create();
var_dump($oid);

/* don't commit */
pg_query($conn, "ROLLBACK");
pg_close($conn);
?>
--EXPECTF--
pg_lo_create(): Argument #1 ($connection) must be of type PgSql\Connection when the connection is provided%w
int(%d)

Deprecated: pg_lo_create(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d
int(%d)
