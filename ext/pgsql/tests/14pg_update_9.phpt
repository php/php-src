--TEST--
PostgreSQL pg_update() (9.0+)
--SKIPIF--
<?php
include("skipif.inc");
skip_bytea_not_hex();
?>
--FILE--
<?php
error_reporting(E_ALL);

include 'config.inc';

$db = pg_connect($conn_str);
pg_query($db, "SET standard_conforming_strings = 0");

$fields = array('num'=>'1234', 'str'=>'ABC', 'bin'=>'XYZ');
$ids = array('num'=>'1234');

pg_update($db, $table_name, $fields, $ids) or print "Error in test 1\n";
echo pg_update($db, $table_name, $fields, $ids, PGSQL_DML_STRING)."\n";
echo pg_update($db, $table_name, $fields, $ids, PGSQL_DML_STRING|PGSQL_DML_ESCAPE)."\n";

echo "Ok\n";
?>
--EXPECT--
UPDATE "php_pgsql_test" SET "num"=1234,"str"=E'ABC',"bin"=E'\\x58595a' WHERE "num"=1234;
UPDATE "php_pgsql_test" SET "num"='1234',"str"='ABC',"bin"='XYZ' WHERE "num"='1234';
Ok