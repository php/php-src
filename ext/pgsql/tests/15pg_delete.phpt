--TEST--
PostgreSQL pg_delete()
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
error_reporting(E_ALL);

include 'config.inc';

$db = pg_connect($conn_str);

$fields = array('num'=>'1234', 'str'=>'XXX', 'bin'=>'YYY');
$ids = array('num'=>'1234');
if (!pg_delete($db, $table_name, $ids)) {
	echo "Error\n";
}
else {
	echo "Ok\n";
}
?>
--EXPECT--
Ok
