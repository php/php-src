--TEST--
PostgreSQL pg_escape_bytea() functions
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
// optional functions

include('config.inc');

$image = file_get_contents(dirname(__FILE__) . '/php.gif');
$esc_image = pg_escape_bytea($image);

$db = pg_connect($conn_str);
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
