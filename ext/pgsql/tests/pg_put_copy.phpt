--TEST--
PostgreSQL pg_put_copy_data/pg_end_copy_end
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');
$db = pg_connect($conn_str);

pg_query($db, "CREATE TABLE copystdin(id int, val text)");
pg_query($db, "COPY copystdin(id, val) FROM STDIN");

var_dump(pg_put_copy_data($db, "0\tFooBar\n"));
var_dump(pg_put_copy_data($db, "1\tBarFoo\n"));
var_dump(pg_put_copy_data($db, ""));
var_dump(pg_put_copy_end($db));

var_dump(pg_get_result($db));

var_dump(pg_fetch_all(pg_query($db, "SELECT * FROM copystdin ORDER BY id")));
var_dump(pg_put_copy_data($db, "Error\t1"));
var_dump(pg_put_copy_end($db));
var_dump(pg_last_error($db));

pg_query($db, "COPY copystdin(id, val) FROM STDIN");
var_dump(pg_put_copy_data($db, "Error\t1"));
var_dump(pg_put_copy_end($db));

var_dump(pg_get_result($db));

var_dump(pg_fetch_all(pg_query($db, "SELECT * FROM copystdin ORDER BY id")));
?>
--CLEAN--
<?php
include('inc/config.inc');

$db = pg_connect($conn_str);
pg_query($db, "DROP TABLE IF EXISTS copystdin");
?>
--EXPECT--
int(1)
int(1)
int(1)
int(1)
object(PgSql\Result)#2 (0) {
}
array(2) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "0"
    ["val"]=>
    string(6) "FooBar"
  }
  [1]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["val"]=>
    string(6) "BarFoo"
  }
}
int(-1)
int(-1)
string(39) "no COPY in progress
no COPY in progress"
int(1)
int(1)
object(PgSql\Result)#2 (0) {
}
array(2) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "0"
    ["val"]=>
    string(6) "FooBar"
  }
  [1]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["val"]=>
    string(6) "BarFoo"
  }
}
