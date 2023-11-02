--TEST--
Bug #37100 (data is returned truncated with BINARY CURSOR) (9.0+)
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
include("inc/skipif.inc");
skip_bytea_not_hex();
?>
--FILE--
<?php

include 'inc/config.inc';

$db = pg_connect($conn_str);

pg_query($db, 'CREATE TABLE test_bug (binfield byteA) ;');
pg_query($db, "INSERT INTO test_bug VALUES (decode('0103AA000812','hex'))");


$data = pg_query($db, "SELECT binfield FROM test_bug");
$res = pg_fetch_result($data, null, 0);
var_dump($res);
var_dump(bin2hex(pg_unescape_bytea($res)));

$sql = "BEGIN; DECLARE mycursor BINARY CURSOR FOR SELECT binfield FROM test_bug; FETCH ALL IN mycursor;";

$data = pg_query($db, $sql);
$res = pg_fetch_result($data, null, 0);

var_dump(strlen($res));
var_dump(bin2hex($res));

pg_close($db);

$db = pg_connect($conn_str);
pg_close($db);


?>
--CLEAN--
<?php
require_once('inc/config.inc');
$db = pg_connect($conn_str);

pg_query($db, 'DROP TABLE test_bug');
?>
--EXPECT--
string(14) "\x0103aa000812"
string(12) "0103aa000812"
int(6)
string(12) "0103aa000812"
