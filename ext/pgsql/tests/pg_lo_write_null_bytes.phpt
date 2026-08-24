--TEST--
pg_lo_write() must accept data containing null bytes
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php
include('inc/config.inc');

$db = pg_connect($conn_str);
pg_exec($db, "BEGIN");
$oid = pg_lo_create($db);
$handle = pg_lo_open($db, $oid, "w");
var_dump(pg_lo_write($handle, "bin\0ary\0data"));
var_dump(pg_lo_write($handle, "ab\0cd", 4));
var_dump(pg_lo_write($handle, "ab\0cd", 2));
var_dump(pg_lo_write($handle, "", 0));
try {
    pg_lo_write($handle, "abc", -1);
} catch (ValueError $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
try {
    pg_lo_write($handle, "abc", 4);
} catch (ValueError $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
pg_lo_close($handle);
pg_exec($db, "ROLLBACK");

pg_exec($db, "BEGIN");
$oid = pg_lo_create($db);
$handle = pg_lo_open($db, $oid, "w");
pg_lo_write($handle, "x\0y");
pg_lo_close($handle);
$handle = pg_lo_open($db, $oid, "r");
var_dump(bin2hex(pg_lo_read($handle)));
pg_lo_close($handle);
pg_exec($db, "ROLLBACK");
?>
--EXPECT--
int(12)
int(4)
int(2)
int(0)
ValueError: pg_lo_write(): Argument #3 ($length) must be greater than or equal to 0
ValueError: pg_lo_write(): Argument #3 ($length) must be less than or equal to the length of argument #2 ($buf)
string(6) "780079"
