--TEST--
pg_copy_to() / pg_copy_from() accept only plain identifier or schema.table
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');

$db = pg_connect($conn_str);
pg_query($db, 'DROP SCHEMA IF EXISTS copyv_schema CASCADE');
pg_query($db, 'DROP TABLE IF EXISTS pg_copy_v_plain');
pg_query($db, 'CREATE TABLE pg_copy_v_plain (v text)');
pg_query($db, 'CREATE SCHEMA copyv_schema');
pg_query($db, 'CREATE TABLE copyv_schema.t (v text)');

echo "--- accepted: plain identifier ---\n";
var_dump(pg_copy_from($db, 'pg_copy_v_plain', ["a\n"]));
var_dump(pg_copy_to($db, 'pg_copy_v_plain'));

echo "--- accepted: schema.table ---\n";
var_dump(pg_copy_from($db, 'copyv_schema.t', ["b\n"]));
var_dump(pg_copy_to($db, 'copyv_schema.t'));

echo "--- rejected: column list ---\n";
var_dump(pg_copy_from($db, 'pg_copy_v_plain (v)', ["c\n"]));
var_dump(pg_copy_to($db, 'pg_copy_v_plain (v)'));

echo "--- rejected: ONLY ---\n";
var_dump(pg_copy_from($db, 'ONLY pg_copy_v_plain', ["d\n"]));
var_dump(pg_copy_to($db, 'ONLY pg_copy_v_plain'));

echo "--- rejected: quoted identifier ---\n";
var_dump(pg_copy_from($db, '"pg_copy_v_plain"', ["e\n"]));
var_dump(pg_copy_to($db, '"pg_copy_v_plain"'));

echo "--- rejected: quoted name with literal dot ---\n";
var_dump(pg_copy_from($db, '"my.table"', ["f\n"]));
var_dump(pg_copy_to($db, '"my.table"'));

echo "--- rejected: leading digit ---\n";
var_dump(pg_copy_from($db, '1pg_copy_v_plain', ["g\n"]));

echo "--- rejected: trailing dot ---\n";
var_dump(pg_copy_from($db, 'pg_copy_v_plain.', ["h\n"]));

echo "--- rejected: empty ---\n";
var_dump(pg_copy_from($db, '', ["i\n"]));

echo "--- final row counts ---\n";
var_dump(pg_num_rows(pg_query($db, 'SELECT * FROM pg_copy_v_plain')));
var_dump(pg_num_rows(pg_query($db, 'SELECT * FROM copyv_schema.t')));

?>
--CLEAN--
<?php
include('inc/config.inc');
$db = pg_connect($conn_str);
pg_query($db, 'DROP SCHEMA IF EXISTS copyv_schema CASCADE');
pg_query($db, 'DROP TABLE IF EXISTS pg_copy_v_plain');
?>
--EXPECTF--
--- accepted: plain identifier ---
bool(true)
array(1) {
  [0]=>
  string(2) "a
"
}
--- accepted: schema.table ---
bool(true)
array(1) {
  [0]=>
  string(2) "b
"
}
--- rejected: column list ---

Warning: pg_copy_from(): Invalid table_name 'pg_copy_v_plain (v)': must be a plain identifier or schema.table in %s on line %d
bool(false)

Warning: pg_copy_to(): Invalid table_name 'pg_copy_v_plain (v)': must be a plain identifier or schema.table in %s on line %d
bool(false)
--- rejected: ONLY ---

Warning: pg_copy_from(): Invalid table_name 'ONLY pg_copy_v_plain': must be a plain identifier or schema.table in %s on line %d
bool(false)

Warning: pg_copy_to(): Invalid table_name 'ONLY pg_copy_v_plain': must be a plain identifier or schema.table in %s on line %d
bool(false)
--- rejected: quoted identifier ---

Warning: pg_copy_from(): Invalid table_name '"pg_copy_v_plain"': must be a plain identifier or schema.table in %s on line %d
bool(false)

Warning: pg_copy_to(): Invalid table_name '"pg_copy_v_plain"': must be a plain identifier or schema.table in %s on line %d
bool(false)
--- rejected: quoted name with literal dot ---

Warning: pg_copy_from(): Invalid table_name '"my.table"': must be a plain identifier or schema.table in %s on line %d
bool(false)

Warning: pg_copy_to(): Invalid table_name '"my.table"': must be a plain identifier or schema.table in %s on line %d
bool(false)
--- rejected: leading digit ---

Warning: pg_copy_from(): Invalid table_name '1pg_copy_v_plain': must be a plain identifier or schema.table in %s on line %d
bool(false)
--- rejected: trailing dot ---

Warning: pg_copy_from(): Invalid table_name 'pg_copy_v_plain.': must be a plain identifier or schema.table in %s on line %d
bool(false)
--- rejected: empty ---

Warning: pg_copy_from(): Invalid table_name '': must be a plain identifier or schema.table in %s on line %d
bool(false)
--- final row counts ---
int(1)
int(1)
