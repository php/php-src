--TEST--
SQLite3::createFunction - Basic test
--EXTENSIONS--
sqlite3
--FILE--
<?php

require_once(__DIR__ . '/new_db.inc');

$func = 'strtoupper';
var_dump($db->createFunction($func, $func));
var_dump($db->querySingle("SELECT strtoupper('test')"));

$func2 = 'strtolower';
var_dump($db->createFunction($func2, $func2));
var_dump($db->querySingle("SELECT strtolower('TEST')"));

var_dump($db->createFunction($func, $func2));
var_dump($db->querySingle("SELECT strtoupper('tEst')"));


?>
--EXPECT--
bool(true)
string(4) "TEST"
bool(true)
string(4) "test"
bool(true)
string(4) "test"
