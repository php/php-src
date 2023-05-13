--TEST--
Bug #77047 pg_insert has a broken regex for the 'TIME WITHOUT TIMEZONE' data type
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
include("skipif.inc");
?>
--FILE--
<?php
error_reporting(E_ALL);

include 'config.inc';

$db = pg_connect($conn_str);

pg_query($db, "DROP TABLE IF EXISTS bug77047");
pg_query($db, "CREATE TABLE bug77047 (
        t TIME WITHOUT TIME ZONE
    )");

try {
	pg_insert($db, "bug77047", array("t" => "13:31"));
	pg_insert($db, "bug77047", array("t" => "13:31:13"));
	pg_insert($db, "bug77047", array("t" => "1:2:3"));
	pg_insert($db, "bug77047", array("t" => "xyz"));
	pg_insert($db, "bug77047", array("t" => NULL));
	pg_insert($db, "bug77047", array("t" => ""));
} catch (\TypeError $e) {
	echo $e->getMessage();
}

$res = pg_query($db, "SELECT t FROM bug77047");
while (false !== ($row = pg_fetch_row($res))) {
	var_dump(array_pop($row));
}

?>
--EXPECTF--
Expects string or null for PostgreSQL 'time' (t)string(%d) "%s"
string(8) "13:31:13"
string(8) "01:02:03"
