<?php
error_reporting(E_ALL);

include 'config.inc';

$db = pg_connect($conn_str);

$meta = pg_meta_data($db, $table_name);

var_dump($meta);


?>
