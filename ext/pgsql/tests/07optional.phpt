--TEST--
PostgreSQL optional functions
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
// optional functions

include('config.inc');

$db = pg_connect($conn_str);
$enc = pg_client_encoding($db);

pg_set_client_encoding($db, $enc);

echo "OK";
?>
--EXPECT--
OK
