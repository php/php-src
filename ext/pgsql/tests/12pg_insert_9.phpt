--TEST--
PostgreSQL pg_insert() (9.0+)
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

$fields = array('num'=>'1234', 'str'=>'AAA', 'bin'=>'BBB');

pg_insert($db, $table_name, $fields) or print "Error in test 1\n";
echo pg_insert($db, $table_name, $fields, PGSQL_DML_STRING)."\n";
echo pg_insert($db, $table_name, $fields, PGSQL_DML_STRING|PGSQL_DML_ESCAPE)."\n";
var_dump( pg_insert($db, $table_name, $fields, PGSQL_DML_EXEC) ); // Return resource

echo "Ok\n";
?>
--EXPECTF--
INSERT INTO "php_pgsql_test" ("num","str","bin") VALUES (1234,E'AAA',E'\\x424242');
INSERT INTO "php_pgsql_test" ("num","str","bin") VALUES ('1234','AAA','BBB');
resource(%d) of type (pgsql result)
Ok
