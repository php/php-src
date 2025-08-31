--TEST--
PostgreSQL pg_escape_bytea() functions (before connection)
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php
// optional functions

include('inc/config.inc');
$table_name = "table_18pg_escape_bytea_before";

$image = file_get_contents(__DIR__ . '/php.gif');
$esc_image = pg_escape_bytea($image);

$db = pg_connect($conn_str);
pg_query($db, "CREATE TABLE {$table_name} (num int, str text, bin bytea)");

@pg_query($db, "SET bytea_output = 'escape'");

pg_query($db, 'INSERT INTO '.$table_name.' (num, bin) VALUES (9876, E\''.$esc_image.'\');');
$result = pg_query($db, 'SELECT * FROM '.$table_name.' WHERE num = 9876');
$rows = pg_fetch_all($result);
$unesc_image = pg_unescape_bytea($rows[0]['bin']);

if ($unesc_image !== $image) {
    echo "NG";
}
else {
    echo "OK";
}
?>
--CLEAN--
<?php
include('inc/config.inc');
$table_name = "table_18pg_escape_bytea_before";

$db = pg_connect($conn_str);
pg_query($db, "DROP TABLE IF EXISTS {$table_name}");
?>
--EXPECTF--
Deprecated: pg_escape_bytea(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d
OK
