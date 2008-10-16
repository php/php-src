--TEST--
PostgreSQL pg_meta_data() - basic test using schema
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

include('config.inc');

$conn = pg_connect($conn_str);

pg_query('CREATE SCHEMA phptests');

pg_query('CREATE TABLE phptests.foo (id INT, id2 INT)');

pg_query('CREATE TABLE foo (id INT, id3 INT)');


var_dump(pg_meta_data($conn, 'foo'));
var_dump(pg_meta_data($conn, 'phptests.foo'));


pg_query('DROP TABLE foo');
pg_query('DROP TABLE phptests.foo');
pg_query('DROP SCHEMA phptests');

?>
--EXPECT--
array(2) {
  ["id"]=>
  array(6) {
    ["num"]=>
    int(1)
    ["type"]=>
    string(4) "int4"
    ["len"]=>
    int(4)
    ["not null"]=>
    bool(false)
    ["has default"]=>
    bool(false)
    ["array dims"]=>
    int(0)
  }
  ["id3"]=>
  array(6) {
    ["num"]=>
    int(2)
    ["type"]=>
    string(4) "int4"
    ["len"]=>
    int(4)
    ["not null"]=>
    bool(false)
    ["has default"]=>
    bool(false)
    ["array dims"]=>
    int(0)
  }
}
array(2) {
  ["id"]=>
  array(6) {
    ["num"]=>
    int(1)
    ["type"]=>
    string(4) "int4"
    ["len"]=>
    int(4)
    ["not null"]=>
    bool(false)
    ["has default"]=>
    bool(false)
    ["array dims"]=>
    int(0)
  }
  ["id2"]=>
  array(6) {
    ["num"]=>
    int(2)
    ["type"]=>
    string(4) "int4"
    ["len"]=>
    int(4)
    ["not null"]=>
    bool(false)
    ["has default"]=>
    bool(false)
    ["array dims"]=>
    int(0)
  }
}
