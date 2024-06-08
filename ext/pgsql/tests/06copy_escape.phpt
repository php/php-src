--TEST--
PostgreSQL copy with escape settings
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');
$table_name = "table_06copy";

$db = pg_connect($conn_str);
pg_query($db, "CREATE TABLE {$table_name} (num int, str text, comment text)");
pg_query($db, "INSERT INTO {$table_name} VALUES (0, '\"test A\"', '\"just testing\"')");
pg_query($db, "INSERT INTO {$table_name} VALUES (1, '\"test  B\"', '\"test again\"')");

var_dump(pg_copy_to($db, $table_name, ";", "\\\\N", "%"));

pg_query($db, "DELETE FROM $table_name");

?>
--CLEAN--
<?php
include('inc/config.inc');
$table_name = "table_06copy";

$db = pg_connect($conn_str);
pg_query($db, "DROP TABLE IF EXISTS {$table_name}");
?>
--EXPECT--
array(2) {
  [0]=>
  string(34) "0;"%"test A%"";"%"just testing%""
"
  [1]=>
  string(33) "1;"%"test  B%"";"%"test again%""
"
}
