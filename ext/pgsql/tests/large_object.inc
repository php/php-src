<?php

include('config.inc');

$db = pg_connect("host=localhost");
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
pg_lo_unlink($oid);
pg_exec ($db, "commit");

echo "OK";

?>