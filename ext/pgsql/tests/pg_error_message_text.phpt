--TEST--
The row and large object diagnostics name real parameters and print no object address
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');
$table_name = 'table_pg_error_message_text';

$db = pg_connect($conn_str);
pg_query($db, "CREATE TABLE {$table_name} (id INT)");
pg_query($db, "INSERT INTO {$table_name} VALUES (1)");

$result = pg_query($db, "SELECT id FROM {$table_name}");

/* the row is out of range: the warning used to append a result index read
 * off the PgSql\Result object zval, which was an address, not an index */
var_dump(pg_fetch_result($result, 99, 0));
var_dump(pg_fetch_row($result, 99));
var_dump(pg_field_prtlen($result, 99, 'id'));
var_dump(pg_field_is_null($result, 99, 'id'));

pg_query($db, "BEGIN");
$oid = pg_lo_create($db);
$lob = pg_lo_open($db, $oid, "w");
pg_lo_write($lob, "hi");

try {
    pg_lo_write($lob, "hi", 100);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    pg_lo_read($lob, -1);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

pg_lo_close($lob);
pg_lo_unlink($db, $oid);
pg_query($db, "COMMIT");

pg_query($db, "DROP TABLE {$table_name}");
?>
--EXPECTF--
Warning: pg_fetch_result(): Unable to jump to row 99 in %s on line %d
bool(false)

Warning: pg_fetch_row(): Unable to jump to row 99 in %s on line %d
bool(false)

Warning: pg_field_prtlen(): Unable to jump to row 99 in %s on line %d
bool(false)

Warning: pg_field_is_null(): Unable to jump to row 99 in %s on line %d
bool(false)
pg_lo_write(): Argument #3 ($length) must be less than or equal to the length of argument #2 ($data)
pg_lo_read(): Argument #2 ($length) must be greater than or equal to 0
