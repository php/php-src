--TEST--
PostgreSQL pg_insert()
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
include("inc/skipif.inc");
skip_bytea_not_escape();
?>
--FILE--
<?php
error_reporting(E_ALL);

include 'inc/config.inc';

$db = pg_connect($conn_str);
$fields = array('num'=>'1234', 'str'=>'AAA', 'bin'=>'BBB');

pg_insert($db, $table_name, $fields) or print "Error in test 1\n";
echo pg_insert($db, $table_name, $fields, PGSQL_DML_STRING)."\n";

echo "Ok\n";
?>
--EXPECT--
INSERT INTO "php_pgsql_test" ("num","str","bin") VALUES (1234,E'AAA',E'BBB');
Ok
