<?php
include 'config.inc';

$db = pg_connect($conn_str);
pg_query($db, "BEGIN;");
pg_query($db, "BEGIN;");

$msg = pg_last_notice($db);
echo $msg;
echo "pg_last_notice() is Ok\n";

?>
