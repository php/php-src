--TEST--
PostgreSQL create large object with given oid
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
include("skipif.inc");
$v = pg_version($conn);
if (version_compare("8.3", $v["client"]) > 0) die("skip - requires pg client >= 8.3\n");
if (version_compare("8.3", $v["server"]) > 0) die("skip - requires pg server >= 8.3\n");
?>
--FILE--
<?php

include('config.inc');

$db = pg_connect($conn_str);

echo "create LO from int\n";
pg_exec ($db, "begin");
$oid = pg_lo_create ($db, 21000);
if (!$oid) echo ("pg_lo_create() error\n");
if ($oid != 21000) echo ("pg_lo_create() wrong id\n");
pg_lo_unlink ($db, $oid);
pg_exec ($db, "commit");

echo "create LO from string\n";
pg_exec ($db, "begin");
$oid = pg_lo_create ($db, "21001");
if (!$oid) echo ("pg_lo_create() error\n");
if ($oid != 21001) echo ("pg_lo_create() wrong id\n");
pg_lo_unlink ($db, $oid);
pg_exec ($db, "commit");

echo "OK";
?>
--EXPECTF--
create LO from int
create LO from string
OK
