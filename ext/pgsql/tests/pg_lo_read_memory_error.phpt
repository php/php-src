--TEST--
pg_lo_read() throws a catchable MemoryError when the buffer length cannot fit in the memory limit
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--INI--
memory_limit=64M
opcache.enable_cli=0
--FILE--
<?php

include('inc/config.inc');

$db = pg_connect($conn_str);
pg_query($db, 'BEGIN');
$oid = pg_lo_create($db);
$lo = pg_lo_open($db, $oid, 'w');
pg_lo_write($lo, 'ping');
pg_lo_close($lo);

$lo = pg_lo_open($db, $oid, 'r');

try {
    pg_lo_read($lo, PHP_INT_MAX - 100);
} catch (MemoryError $e) {
    echo $e::class . ': ' . $e->getMessage() . "\n";
}

var_dump(pg_lo_read($lo, 4));

pg_lo_close($lo);
pg_query($db, 'COMMIT');
pg_lo_unlink($db, $oid);
pg_close($db);

?>
--EXPECT--
MemoryError: The resulting string is too large to fit in the configured memory limit
string(4) "ping"
