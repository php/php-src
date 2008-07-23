--TEST--
PostgreSQL large object with given oid
--SKIPIF--
<?php include("skipif.inc"); ?>
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

echo "create LO using default connection\n";
pg_exec ("begin");
$oid = pg_lo_create (21002);
if (!$oid) echo ("pg_lo_create() error\n");
if ($oid != 21002) echo ("pg_lo_create() wrong id\n");
pg_lo_unlink ($oid);
pg_exec ("commit");

echo "import LO from int\n";
pg_exec($db, 'begin');
$oid = pg_lo_import($db, __FILE__, 21003);
if (!$oid) echo ("pg_lo_import() error\n");
if ($oid != 21003) echo ("pg_lo_import() wrong id\n");
pg_lo_unlink ($db, $oid);
pg_exec($db, 'commit');

echo "import LO from string\n";
pg_exec($db, 'begin');
$oid = pg_lo_import($db, __FILE__, "21004");
if (!$oid) echo ("pg_lo_import() error\n");
if ($oid != 21004) echo ("pg_lo_import() wrong id\n");
pg_lo_unlink ($db, $oid);
pg_exec($db, 'commit');

echo "import LO using default connection\n";
pg_exec('begin');
$oid = pg_lo_import($db, __FILE__, 21005);
if (!$oid) echo ("pg_lo_import() error\n");
if ($oid != 21005) echo ("pg_lo_import() wrong id\n");
pg_lo_unlink ($oid);
pg_exec('commit');


echo "OK";
?>
--EXPECT--
create LO from int
create LO from string
create LO using default connection
import LO from int
import LO from string
import LO using default connection
OK
