--TEST--
PostgreSQL pg_update() (8.5+)
--SKIPIF--
<?php
include("skipif.inc");
skip_server_version('8.5dev', '<');
?>
--FILE--
<?php
error_reporting(E_ALL);

include 'config.inc';

$db = pg_connect($conn_str);
$fields = array('num'=>'1234', 'str'=>'ABC', 'bin'=>'XYZ');
$ids = array('num'=>'1234');

pg_update($db, $table_name, $fields, $ids) or print "Error in test 1\n";
echo pg_update($db, $table_name, $fields, $ids, PGSQL_DML_STRING)."\n";

echo "Ok\n";
?>
--EXPECT--
UPDATE php_pgsql_test SET num=1234,str='ABC',bin='\\x58595a' WHERE num=1234;
Ok
