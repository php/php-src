--TEST--
Github bug 10672 (pg_lo_open segfaults in the strict_types mode)
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
include("skipif.inc");
?>
--FILE--
<?php
declare(strict_types=1);
include('config.inc');

$conn = pg_connect($conn_str);

if ($conn === false) {
    die(pg_last_error());
}

pg_query($conn, 'BEGIN');

$oid = pg_lo_create($conn);

if ($oid === false) {
    die(pg_last_error($conn));
}

$lob = pg_lo_open($conn, $oid, 'w');

if ($oid === false) {
    die(pg_last_error($conn));
}

pg_query($conn, 'COMMIT');

echo 'The large object has been opened successfully.', PHP_EOL;
?>
--EXPECT--
The large object has been opened successfully.
