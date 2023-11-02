--TEST--
PostgreSQL pg_delete() - basic test using schema
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');

$conn = pg_connect($conn_str);

pg_query($conn, 'CREATE SCHEMA phptests');

pg_query($conn, 'CREATE TABLE foo (id INT, id2 INT)');
pg_query($conn, 'CREATE TABLE phptests.foo (id INT, id2 INT)');

pg_insert($conn, 'foo', array('id' => 1, 'id2' => 1));
pg_insert($conn, 'foo', array('id' => 1, 'id2' => 2));
pg_insert($conn, 'foo', array('id' => 1, 'id2' => 2));
pg_insert($conn, 'foo', array('id' => 3, 'id2' => 3));

pg_insert($conn, 'phptests.foo', array('id' => 1, 'id2' => 1));
pg_insert($conn, 'phptests.foo', array('id' => 1, 'id2' => 2));
pg_insert($conn, 'phptests.foo', array('id' => 2, 'id2' => 3));
pg_insert($conn, 'phptests.foo', array('id' => 2, 'id2' => 3));

pg_delete($conn, 'foo', array('id' => 1, 'id2' => 0));
pg_delete($conn, 'foo', array('id' => 1, 'id2' => 2));
var_dump(pg_delete($conn, 'foo', array('id' => 1, 'id2' => 2), PGSQL_DML_STRING));

pg_delete($conn, 'phptests.foo', array('id' => 2, 'id2' => 1));
pg_delete($conn, 'phptests.foo', array('id' => 2, 'id2' => 3));
var_dump(pg_delete($conn, 'phptests.foo', array('id' => 2, 'id2' => 3), PGSQL_DML_STRING));

var_dump(pg_fetch_all(pg_query($conn, 'SELECT * FROM foo')));
var_dump(pg_fetch_all(pg_query($conn, 'SELECT * FROM phptests.foo')));

/* Inexistent */
pg_delete($conn, 'bar', array('id' => 1, 'id2' => 2));
var_dump(pg_delete($conn, 'bar', array('id' => 1, 'id2' => 2), PGSQL_DML_STRING));

pg_query($conn, 'DROP TABLE foo');
pg_query($conn, 'DROP TABLE phptests.foo');
pg_query($conn, 'DROP SCHEMA phptests');

?>
--EXPECTF--
string(43) "DELETE FROM "foo" WHERE "id"=1 AND "id2"=2;"
string(54) "DELETE FROM "phptests"."foo" WHERE "id"=2 AND "id2"=3;"
array(2) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["id2"]=>
    string(1) "1"
  }
  [1]=>
  array(2) {
    ["id"]=>
    string(1) "3"
    ["id2"]=>
    string(1) "3"
  }
}
array(2) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["id2"]=>
    string(1) "1"
  }
  [1]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["id2"]=>
    string(1) "2"
  }
}

Warning: pg_delete(): Table 'bar' doesn't exists in %s on line %d

Warning: pg_delete(): Table 'bar' doesn't exists in %s on line %d
bool(false)
