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
pg_query('INSERT INTO phptests.foo VALUES (1,2)');
pg_query('INSERT INTO phptests.foo VALUES (2,3)');

pg_query('CREATE TABLE phptests.bar (id4 INT, id3 INT)');
pg_query('INSERT INTO phptests.bar VALUES (4,5)');
pg_query('INSERT INTO phptests.bar VALUES (6,7)');

/* Inexistent table */
var_dump(pg_select($conn, 'foo', array('id' => 1)));

/* Existent column */
var_dump(pg_select($conn, 'phptests.foo', array('id' => 1)));

/* Testing with inexistent column */
var_dump(pg_select($conn, 'phptests.bar', array('id' => 1)));

/* Existent column */
var_dump(pg_select($conn, 'phptests.bar', array('id4' => 4)));

/* Use a different result type */
var_dump(pg_select($conn, 'phptests.bar', array('id4' => 4), 0, PGSQL_NUM));

pg_query('DROP TABLE phptests.foo');
pg_query('DROP TABLE phptests.bar');
pg_query('DROP SCHEMA phptests');

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
