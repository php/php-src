--TEST--
Bug #37100 (data is returned truncated with BINARY CURSOR) (8.5+)
--SKIPIF--
<?php
include("skipif.inc");
skip_server_version('8.5dev', '<');
?>
--FILE--
<?php

include 'config.inc';

$db = pg_connect($conn_str);

@pg_query('DROP TABLE test_bug');

pg_query('CREATE TABLE test_bug (binfield byteA) ;');
pg_query("INSERT INTO test_bug VALUES (decode('0103AA000812','hex'))");


$data = pg_query("SELECT binfield FROM test_bug");
$res = pg_fetch_result($data,0);
var_dump($res);
var_dump(bin2hex(pg_unescape_bytea($res)));

$sql = "BEGIN; DECLARE mycursor BINARY CURSOR FOR SELECT binfield FROM test_bug; FETCH ALL IN mycursor;";

$data = pg_query($sql);
$res = pg_fetch_result($data,0);

var_dump(strlen($res));
var_dump(bin2hex($res));

pg_close($db);

$db = pg_connect($conn_str);
pg_query('DROP TABLE test_bug');
pg_close($db);


?>
--EXPECT--
string(14) "\x0103aa000812"
string(12) "0103aa000812"
int(6)
string(12) "0103aa000812"
