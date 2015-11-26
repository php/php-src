--TEST--
PostgreSQL large object
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

include('config.inc');

$db = pg_connect($conn_str);

echo "create/write/close LO\n";
pg_exec ($db, "begin");
$oid = pg_lo_create ($db);
if (!$oid) echo ("pg_lo_create() error\n");
$handle = pg_lo_open ($db, $oid, "w");
if (!$handle) echo ("pg_lo_open() error\n");
pg_lo_write ($handle, "large object data\n");
pg_lo_close ($handle);
pg_exec ($db, "commit");

echo "open/read/tell/seek/close LO\n";
pg_exec ($db, "begin");
$handle = pg_lo_open ($db, $oid, "w");
pg_lo_read($handle, 100);
pg_lo_tell($handle);
pg_lo_seek($handle, 2);
pg_lo_close($handle);
pg_exec ($db, "commit");

echo "open/read_all/close LO\n";
pg_exec ($db, "begin");
$handle = pg_lo_open ($db, $oid, "w");
pg_lo_read_all($handle);
if (pg_last_error()) echo "pg_lo_read_all() error\n".pg_last_error();
pg_lo_close($handle);
pg_exec ($db, "commit");

echo "unlink LO\n";
pg_exec ($db, "begin");
pg_lo_unlink($db, $oid) or print("pg_lo_unlink() error 1\n");
pg_exec ($db, "commit");

// more pg_lo_unlink() tests
echo "Test without connection\n";
pg_exec ($db, "begin");
$oid = pg_lo_create ($db) or print("pg_lo_create() error\n");
pg_lo_unlink($oid) or print("pg_lo_unlink() error 2\n");
pg_exec ($db, "commit");

echo "Test with string oid value\n";
pg_exec ($db, "begin");
$oid = pg_lo_create ($db) or print("pg_lo_create() error\n");
pg_lo_unlink($db, (string)$oid) or print("pg_lo_unlink() error 3\n");
pg_exec ($db, "commit");

echo "import/export LO\n";
$path = dirname(__FILE__) . '/';
pg_query($db, 'begin');
$oid = pg_lo_import($db, $path . 'php.gif');
pg_query($db, 'commit');
pg_query($db, 'begin');
@unlink($path . 'php.gif.exported');
pg_lo_export($oid, $path . 'php.gif.exported', $db);
if (!file_exists($path . 'php.gif.exported')) {
	echo "Export failed\n";
}
@unlink($path . 'php.gif.exported');
pg_query($db, 'commit');

echo "OK";
?>
--EXPECT--
create/write/close LO
open/read/tell/seek/close LO
open/read_all/close LO
large object data
unlink LO
Test without connection
Test with string oid value
import/export LO
OK
