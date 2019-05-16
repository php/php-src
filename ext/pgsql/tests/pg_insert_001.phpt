--TEST--
PostgreSQL pg_select() - basic test using schema
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

include('config.inc');

$conn = pg_connect($conn_str);

pg_query('CREATE SCHEMA phptests');
pg_query('CREATE TABLE phptests.foo (id INT, id2 INT)');


pg_insert($conn, 'foo', array('id' => 1, 'id2' => 1));

pg_insert($conn, 'phptests.foo', array('id' => 1, 'id2' => 2));

var_dump(pg_insert($conn, 'phptests.foo', array('id' => 1, 'id2' => 2), PGSQL_DML_STRING));

var_dump(pg_select($conn, 'phptests.foo', array('id' => 1)));

pg_query('DROP TABLE phptests.foo');
pg_query('DROP SCHEMA phptests');

?>
--EXPECTF--
Warning: pg_insert(): Table 'foo' doesn't exists in %s on line %d
string(55) "INSERT INTO "phptests"."foo" ("id","id2") VALUES (1,2);"
array(1) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["id2"]=>
    string(1) "2"
  }
}
