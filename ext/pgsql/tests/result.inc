<?php
error_reporting(E_ALL);

include 'config.inc';

$db = pg_connect($conn_str);

$sql = "SELECT * FROM $table_name";
$result = pg_query($db, $sql) or die('Cannot qeury db');
$rows = pg_num_rows($result);

$rec = pg_fetch_object($result, 1);
var_dump($rec);
$rec = pg_fetch_array($result, 1);
var_dump($rec);
$rec = pg_fetch_row($result, 1);
var_dump($rec);
$rec = pg_fetch_assoc($result);
var_dump($rec);
var_dump(pg_result_seek($result, 0));

echo "Ok\n";

?>