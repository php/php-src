--TEST--
Bug #72197 pg_lo_create arbitrary read
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
pg_lo_create(): supplied resource is not a valid PostgreSQL link resource%w
int(%d)
int(%d)
