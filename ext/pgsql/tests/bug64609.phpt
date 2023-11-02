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

$db = pg_connect($conn_str);
pg_query($db, "BEGIN");
pg_query($db, "CREATE TYPE t_enum AS ENUM ('ok', 'ko')");
pg_query($db, "CREATE TABLE test_enum (a t_enum)");

$fields = array('a' => 'ok');
$converted = pg_convert($db, 'test_enum', $fields);

pg_query($db, "ROLLBACK");

var_dump($converted);
?>
--EXPECT--
array(1) {
  [""a""]=>
  string(5) "E'ok'"
}
