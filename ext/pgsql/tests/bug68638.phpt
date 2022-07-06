--TEST--
Bug #68638 pg_update() fails to store infinite values
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

include('config.inc');

$conn = pg_connect($conn_str);

$table='test_68638';

pg_query("CREATE TABLE $table (id INT, value FLOAT)");

pg_insert($conn,$table, array('id' => 1, 'value' => 1.2));
pg_insert($conn,$table, array('id' => 2, 'value' => 10));
pg_insert($conn,$table, array('id' => 3, 'value' => 15));

var_dump(pg_update($conn,$table, array('value' => 'inf'), array('id' => 1), PGSQL_DML_STRING));

pg_update($conn,$table, array('value' => 'inf'), array('id' => 1));
pg_update($conn,$table, array('value' => '-inf'), array('id' => 2));
pg_update($conn,$table, array('value' => '+inf'), array('id' => 3));

$rs = pg_query("SELECT * FROM $table");
while ($row = pg_fetch_assoc($rs)) {
        var_dump($row);
}

pg_query("DROP TABLE $table");

?>
--EXPECT--
string(52) "UPDATE "test_68638" SET "value"=E'inf' WHERE "id"=1;"
array(2) {
  ["id"]=>
  string(1) "1"
  ["value"]=>
  string(8) "Infinity"
}
array(2) {
  ["id"]=>
  string(1) "2"
  ["value"]=>
  string(9) "-Infinity"
}
array(2) {
  ["id"]=>
  string(1) "3"
  ["value"]=>
  string(8) "Infinity"
}
