--TEST--
PostgreSQL pg_delete()
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php
error_reporting(E_ALL);

include 'inc/config.inc';
$table_name = "table_15pg_delete";

$db = pg_connect($conn_str);
pg_query($db, "CREATE TABLE {$table_name} (num int, str text, bin bytea)");
pg_query($db, "INSERT INTO {$table_name} VALUES(1, 'ABC', null)");
pg_query($db, "INSERT INTO {$table_name} VALUES(1, 'ABC', null)");

$fields = array('num'=>'1234', 'str'=>'XXX', 'bin'=>'YYY');
$ids = array('num'=>'1234');
echo pg_delete($db, $table_name, $ids, PGSQL_DML_STRING)."\n";
echo pg_delete($db, $table_name, $ids, PGSQL_DML_STRING|PGSQL_DML_ESCAPE)."\n";
if (!pg_delete($db, $table_name, $ids)) {
    echo "Error\n";
}
else {
    echo "Ok\n";
}
?>
--CLEAN--
<?php
include('inc/config.inc');
$table_name = "table_15pg_delete";

$db = pg_connect($conn_str);
pg_query($db, "DROP TABLE IF EXISTS {$table_name}");
?>
--EXPECT--
DELETE FROM "table_15pg_delete" WHERE "num"=1234;
DELETE FROM "table_15pg_delete" WHERE "num"='1234';
Ok
