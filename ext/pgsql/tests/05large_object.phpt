--TEST--
PostgreSQL large object
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');

$db = pg_connect($conn_str);

echo "create/write/close LO\n";
pg_exec ($db, "BEGIN");
$oid = pg_lo_create ($db);
if (!$oid) echo ("pg_lo_create() error\n");
$handle = pg_lo_open ($db, $oid, "w");
if (!$handle) echo ("pg_lo_open() error\n");
pg_lo_write ($handle, "large object data");
pg_lo_close ($handle);
pg_exec ($db, "COMMIT");

echo "open/read/tell/seek/close LO\n";
pg_exec ($db, "BEGIN");
$handle = pg_lo_open ($db, $oid, "w");
var_dump(pg_lo_read($handle, 5));
var_dump(pg_lo_tell($handle));
var_dump(pg_lo_seek($handle, 2, /* PGSQL_SEEK_CUR */)); // This is the default so move cursor from 5
var_dump(pg_lo_read($handle, 100)); // Read to the end because chunk is larger then remaining content
var_dump(pg_lo_tell($handle));
var_dump(pg_lo_seek($handle, 0, PGSQL_SEEK_SET)); /* Reset cursor to beginning */
var_dump(pg_lo_read($handle));
var_dump(pg_lo_seek($handle, -4, PGSQL_SEEK_END)); /* Seek from the end */
var_dump(pg_lo_read($handle));
pg_lo_close($handle);
pg_exec ($db, "COMMIT");

echo "open/read_all/close LO\n";
pg_exec ($db, "BEGIN");
$handle = pg_lo_open ($db, $oid, "w");
/* Will write to stdout */
$bytesWritten = pg_lo_read_all($handle);
echo "\n";
var_dump($bytesWritten);
if (pg_last_error($db)) echo "pg_lo_read_all() error\n".pg_last_error();
pg_lo_close($handle);
pg_exec ($db, "COMMIT");

echo "unlink LO\n";
pg_exec ($db, "BEGIN");
pg_lo_unlink($db, $oid) or print("pg_lo_unlink() error 1\n");
pg_exec ($db, "COMMIT");

// more pg_lo_unlink() tests
echo "Test without connection\n";
pg_exec ($db, "BEGIN");
$oid = pg_lo_create ($db) or print("pg_lo_create() error\n");
pg_lo_unlink($oid) or print("pg_lo_unlink() error 2\n");
pg_exec ($db, "COMMIT");

echo "Test with string oid value\n";
pg_exec ($db, "BEGIN");
$oid = pg_lo_create ($db) or print("pg_lo_create() error\n");
pg_lo_unlink($db, (string)$oid) or print("pg_lo_unlink() error 3\n");
pg_exec ($db, "COMMIT");

echo "import/export LO\n";
$path = __DIR__ . '/';
pg_query($db, 'BEGIN');
$oid = pg_lo_import($db, $path . 'php.gif');
pg_query($db, 'COMMIT');
pg_query($db, 'BEGIN');
@unlink($path . 'php.gif.exported');
pg_lo_export($db, $oid, $path . 'php.gif.exported');
if (!file_exists($path . 'php.gif.exported')) {
    echo "Export failed\n";
}
@unlink($path . 'php.gif.exported');
pg_query($db, 'COMMIT');

/* invalid OID values */
try {
    pg_lo_create(-15);
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    pg_lo_create($db, -15);
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    pg_lo_create('giberrish');
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    pg_lo_create($db, 'giberrish');
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

echo "OK";
?>
--EXPECTF--
create/write/close LO
open/read/tell/seek/close LO
string(5) "large"
int(5)
bool(true)
string(10) "bject data"
int(17)
bool(true)
string(17) "large object data"
bool(true)
string(4) "data"
open/read_all/close LO
large object data
int(17)
unlink LO
Test without connection

Deprecated: pg_lo_unlink(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d
Test with string oid value
import/export LO

Deprecated: pg_lo_create(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d
Invalid OID value passed
Invalid OID value passed

Deprecated: pg_lo_create(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d
Invalid OID value passed
Invalid OID value passed
OK
