--TEST--
PostgreSQL pg_ping() functions
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php
// optional functions

include('config.inc');

$db = pg_connect($conn_str);
var_dump(pg_ping($db));
?>
--EXPECT--
bool(true)
