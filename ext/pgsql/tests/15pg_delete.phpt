--TEST--
PostgreSQL pg_delete()
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
error_reporting(E_ALL);

include 'config.inc';

$db = pg_connect($conn_str);

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
--EXPECT--
DELETE FROM "php_pgsql_test" WHERE "num"=1234;
DELETE FROM "php_pgsql_test" WHERE "num"='1234';
Ok
