<?php
error_reporting(E_ALL);

include 'config.inc';

$db = pg_connect($conn_str);

$fields = array('num'=>'1234', 'str'=>'XXX', 'bin'=>'YYY');
$ids = array('num'=>'1234');
if (!pg_update($db, $table_name, $fields, $ids, 1, 0)) {
	echo "Error\n";
}
else {
	echo "Ok\n";
}

?>
