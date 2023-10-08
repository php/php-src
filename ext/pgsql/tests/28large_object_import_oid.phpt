--TEST--
PostgreSQL import large object with given oid
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
include("skipif.inc");
$v = pg_version($conn);
if (version_compare("8.4devel", $v["client"]) > 0) die("skip - requires pg client >= 8.4\n");
if (version_compare("8.4devel", $v["server"]) > 0) die("skip - requires pg server >= 8.4\n");
?>
--FILE--
<?php

include('config.inc');

$db = pg_connect($conn_str);

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

/* Invalide OID */
try {
    pg_lo_import(__FILE__, -15);
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    pg_lo_import($db, __FILE__, -15);
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    pg_lo_import(__FILE__, 'giberrish');
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    pg_lo_import($db, __FILE__, 'giberrish');
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    pg_lo_import(__FILE__, true);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    pg_lo_import($db, __FILE__, []);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    pg_lo_import($db, __FILE__, new stdClass());
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    pg_lo_import($db, __FILE__, $db);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

echo "OK";
?>
--EXPECTF--
import LO from int
import LO from string
import LO using default connection

Deprecated: pg_exec(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_lo_unlink(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_exec(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_lo_import(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d
Invalid OID value passed
Invalid OID value passed

Deprecated: pg_lo_import(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d
Invalid OID value passed
Invalid OID value passed

Deprecated: pg_lo_import(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d
OID value must be of type string|int, true given
OID value must be of type string|int, array given
OID value must be of type string|int, stdClass given
OID value must be of type string|int, PgSql\Connection given
OK
