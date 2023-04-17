--TEST--
GH-10672 (pg_lo_open segfaults in the strict_types mode)
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
include("skipif.inc");
?>
--FILE--
<?php
declare(strict_types=1);

include "config.inc";

$db = pg_connect($conn_str);
pg_query($db, "DROP TABLE IF EXISTS gh10672");
pg_query($db, "CREATE TABLE gh10672 (bar text);");

// Begin a transaction
pg_query($db, 'BEGIN');

// Create an empty large object
$oid = pg_lo_create($db);

if ($oid === false) {
    die(pg_last_error($db));
}

// Open the large object for writing
$lob = pg_lo_open($db, $oid, 'w');

if ($oid === false) {
    die(pg_last_error($db));
}

echo 'The large object has been opened successfully.', PHP_EOL;
?>
--EXPECT--
The large object has been opened successfully.
