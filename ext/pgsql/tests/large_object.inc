<?php

include('config.inc');

$db = pg_connect($conn_str);
pg_exec ($db, "begin");
$oid = pg_lo_create ($db);
if (!$oid) echo ("pg_lo_create() error\n");
$handle = pg_lo_open ($db, $oid, "w");
if (!$handle) echo ("pg_lo_open() error\n");
pg_lo_write ($handle, "large object data\n");
pg_lo_close ($handle);
pg_exec ($db, "commit");

pg_exec ($db, "begin");
$handle = pg_lo_open ($db, $oid, "w");
pg_lo_read($handle, 100);
pg_lo_tell($handle);
pg_lo_seek($handle, 2);
pg_lo_close($handle);
pg_exec ($db, "commit");

pg_exec ($db, "begin");
$handle = pg_lo_open ($db, $oid, "w");
pg_lo_read_all($handle);
if (pg_last_error()) echo "pg_lo_read_all() error\n".pg_last_error();
pg_lo_close($handle);
pg_exec ($db, "commit");

pg_exec ($db, "begin");
pg_lo_unlink($db, $oid) or print("pg_lo_unlink() error\n");
pg_exec ($db, "commit");

// more pg_lo_unlink() tests
// Test without connection 
pg_exec ($db, "begin");
$oid = pg_lo_create ($db) or print("pg_lo_create() error\n");
pg_lo_unlink($oid) or print("pg_lo_unlink() error\n");
pg_exec ($db, "commit");

// Test with string oid value
pg_exec ($db, "begin");
$oid = pg_lo_create ($db) or print("pg_lo_create() error\n");
pg_lo_unlink($db, (string)$oid) or print("pg_lo_unlink() error\n");
pg_exec ($db, "commit");

echo "OK";

?>