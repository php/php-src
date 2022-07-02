--TEST--
PostgreSQL pg_escape_bytea() functions (hex format)
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
include("skipif.inc");
skip_bytea_not_hex();
?>
--FILE--
<?php
// optional functions

include('config.inc');

$db = pg_connect($conn_str);
@pg_query($db, "SET bytea_output = 'hex'");

$image = file_get_contents(__DIR__ . '/php.gif');
$esc_image = pg_escape_bytea($db, $image);

pg_query($db, 'INSERT INTO '.$table_name.' (num, bin) VALUES (9876, \''.$esc_image.'\');');
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
--EXPECT--
OK
