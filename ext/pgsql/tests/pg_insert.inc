<?php
error_reporting(E_ALL);

include 'config.inc';

$db = pg_connect($conn_str);

$fields = array('num'=>'1234', 'str'=>'AAA', 'bin'=>'BBB');
if (!pg_insert($db, $table_name, $fields, 1, 0)) {
	echo "Error\n";
}
else {
	echo "Ok\n";
}

?>
