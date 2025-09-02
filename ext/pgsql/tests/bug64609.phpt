--TEST--
Bug #64609 (pg_convert enum type support)
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
include("inc/skipif.inc");
skip_server_version('8.3', '<');
?>
--FILE--
<?php
error_reporting(E_ALL);

include 'inc/config.inc';
$table_name = 'table_bug64609';
$type_name = 'type_bug64609';

$db = pg_connect($conn_str);
pg_query($db, "BEGIN");
pg_query($db, "CREATE TYPE {$type_name} AS ENUM ('ok', 'ko')");
pg_query($db, "CREATE TABLE {$table_name} (a {$type_name})");

$fields = array('a' => 'ok');
$converted = pg_convert($db, $table_name, $fields);

pg_query($db, "ROLLBACK");

var_dump($converted);
?>
--EXPECT--
array(1) {
  [""a""]=>
  string(5) "E'ok'"
}
