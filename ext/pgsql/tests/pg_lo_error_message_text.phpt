--TEST--
The large object diagnostics name real parameters
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');

$db = pg_connect($conn_str);

pg_query($db, "BEGIN");
$oid = pg_lo_create($db);
$lob = pg_lo_open($db, $oid, "w");
pg_lo_write($lob, "hi");

try {
    pg_lo_write($lob, "hi", 100);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    pg_lo_read($lob, -1);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

pg_lo_close($lob);
pg_lo_unlink($db, $oid);
pg_query($db, "COMMIT");
?>
--EXPECT--
pg_lo_write(): Argument #3 ($length) must be less than or equal to the length of argument #2 ($data)
pg_lo_read(): Argument #2 ($length) must be greater than or equal to 0
