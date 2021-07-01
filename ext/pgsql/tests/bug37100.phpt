--TEST--
Bug #37100 (data is returned truncated with BINARY CURSOR)
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
include("skipif.inc");
skip_bytea_not_escape();
?>
--FILE--
<?php

include 'config.inc';

$db = pg_connect($conn_str);
@pg_query("SET bytea_output = 'escape'");

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
string(24) "\001\003\252\000\010\022"
string(12) "0103aa000812"
int(6)
string(12) "0103aa000812"
