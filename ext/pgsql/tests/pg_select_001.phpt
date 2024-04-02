--TEST--
PostgreSQL pg_select() - basic test using schema
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');
$schema_name = 'schema_pg_select_001';

$conn = pg_connect($conn_str);

pg_query($conn, "CREATE SCHEMA {$schema_name}");

pg_query($conn, "CREATE TABLE {$schema_name}.foo (id INT, id2 INT)");
pg_query($conn, "INSERT INTO {$schema_name}.foo VALUES (1,2)");
pg_query($conn, "INSERT INTO {$schema_name}.foo VALUES (2,3)");

pg_query($conn, "CREATE TABLE {$schema_name}.bar (id4 INT, id3 INT)");
pg_query($conn, "INSERT INTO {$schema_name}.bar VALUES (4,5)");
pg_query($conn, "INSERT INTO {$schema_name}.bar VALUES (6,7)");

/* Nonexistent table */
var_dump(pg_select($conn, 'foo', array('id' => 1)));

/* Existent column */
var_dump(pg_select($conn, "{$schema_name}.foo", array('id' => 1)));

/* Testing with inexistent column */
var_dump(pg_select($conn, "{$schema_name}.bar", array('id' => 1)));

/* Existent column */
var_dump(pg_select($conn, "{$schema_name}.bar", array('id4' => 4)));

/* Use a different result type */
var_dump(pg_select($conn, "{$schema_name}.bar", array('id4' => 4), 0, PGSQL_NUM));

/* Empty array */
var_dump(pg_select($conn, "{$schema_name}.bar", array()));

/* No array */
var_dump(pg_select($conn, "{$schema_name}.bar"));

?>
--CLEAN--
<?php
require_once('inc/config.inc');
$schema_name = 'schema_pg_select_001';

$conn = pg_connect($conn_str);

pg_query($conn, "DROP TABLE IF EXISTS {$schema_name}.foo");
pg_query($conn, "DROP TABLE IF EXISTS {$schema_name}.bar");
pg_query($conn, "DROP SCHEMA IF EXISTS {$schema_name}");
?>
--EXPECTF--
Warning: pg_select(): Table 'foo' doesn't exists in %s on line %d
bool(false)
array(1) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["id2"]=>
    string(1) "2"
  }
}

Notice: pg_select(): Invalid field name (id) in values in %s on line %d
bool(false)
array(1) {
  [0]=>
  array(2) {
    ["id4"]=>
    string(1) "4"
    ["id3"]=>
    string(1) "5"
  }
}
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "4"
    [1]=>
    string(1) "5"
  }
}
array(2) {
  [0]=>
  array(2) {
    ["id4"]=>
    string(1) "4"
    ["id3"]=>
    string(1) "5"
  }
  [1]=>
  array(2) {
    ["id4"]=>
    string(1) "6"
    ["id3"]=>
    string(1) "7"
  }
}
array(2) {
  [0]=>
  array(2) {
    ["id4"]=>
    string(1) "4"
    ["id3"]=>
    string(1) "5"
  }
  [1]=>
  array(2) {
    ["id4"]=>
    string(1) "6"
    ["id3"]=>
    string(1) "7"
  }
}
