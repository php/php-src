--TEST--
GH-10672 (pg_lo_open segfaults in the strict_types mode)
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
include("inc/skipif.inc");
?>
--FILE--
<?php
declare(strict_types=1);

include "inc/config.inc";
$table_name = 'table_gh10672';

$db = pg_connect($conn_str);
pg_query($db, "CREATE TABLE {$table_name} (bar text);");

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
--CLEAN--
<?php
require_once('inc/config.inc');
$table_name = 'table_gh10672';

$dbh = pg_connect($conn_str);
pg_query($dbh, "DROP TABLE IF EXISTS {$table_name}");
?>
--EXPECT--
The large object has been opened successfully.
