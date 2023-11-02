--TEST--
PostgreSQL pg_meta_data() - basic test using schema
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');

$conn = pg_connect($conn_str);

pg_query($conn, 'CREATE SCHEMA phptests');

pg_query($conn, 'CREATE TABLE phptests.foo (id INT, id2 INT)');

pg_query($conn, 'CREATE TABLE foo (id INT, id3 INT)');


var_dump(pg_meta_data($conn, 'foo'));
var_dump(pg_meta_data($conn, 'phptests.foo'));
var_dump(pg_meta_data($conn, 'phptests.foo', TRUE));


pg_query($conn, 'DROP TABLE foo');
pg_query($conn, 'DROP TABLE phptests.foo');
pg_query($conn, 'DROP SCHEMA phptests');

?>
--EXPECT--
array(2) {
  ["id"]=>
  array(7) {
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
    ["is enum"]=>
    bool(false)
  }
  ["id3"]=>
  array(7) {
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
    ["is enum"]=>
    bool(false)
  }
}
array(2) {
  ["id"]=>
  array(7) {
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
    ["is enum"]=>
    bool(false)
  }
  ["id2"]=>
  array(7) {
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
    ["is enum"]=>
    bool(false)
  }
}
array(2) {
  ["id"]=>
  array(11) {
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
    ["is enum"]=>
    bool(false)
    ["is base"]=>
    bool(true)
    ["is composite"]=>
    bool(false)
    ["is pseudo"]=>
    bool(false)
    ["description"]=>
    string(0) ""
  }
  ["id2"]=>
  array(11) {
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
    ["is enum"]=>
    bool(false)
    ["is base"]=>
    bool(true)
    ["is composite"]=>
    bool(false)
    ["is pseudo"]=>
    bool(false)
    ["description"]=>
    string(0) ""
  }
}
